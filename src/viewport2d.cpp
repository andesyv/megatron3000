#include "viewport2d.h"
#include "renderer2d.h"
#include <QVBoxLayout>
#include <QMenuBar>
#include <QDebug>
#include "mainwindow.h"
#include "volume.h"
#include "datawidget.h"

Viewport2D::Viewport2D(QWidget *parent) :
    QWidget{parent}, IMenu{this}
{
    /// Apparantly it's easier to just make the ui from code..
    // ui->setupUi(this);

    // Layout:
    mLayout = new QVBoxLayout{this};
    mLayout->setContentsMargins(0, 0, 0, 0);

    // Menubar:
    /// (View menu created in IMenu interface)
    auto orthoMenu = mViewMenu->addMenu("Orthogonal Axis");
    mAxisActions = {
        orthoMenu->addAction("Z Axis"),
        orthoMenu->addAction("X Axis"),
        orthoMenu->addAction("Y Axis"),
    };
    mAxisActions.push_back(mViewMenu->addAction("Arbitrary Axis"));
    mAxisActions.push_back(mViewMenu->addAction("Slice Axis"));
    
    for (auto action : mAxisActions) {
        action->setCheckable(true);
        connect(action, &QAction::triggered, [&, action](){ setAxis(action); });
    }
    mAxisActions.front()->setChecked(true);
    
    mLayout->addWidget(mMenuBar);

    // OpenGL Render Widget:
    mRenderer = new Renderer2D{this};
    if (mVolume)
        mRenderer->mPrivateVolume = mVolume;
    mLayout->addWidget(mRenderer);



    setLayout(mLayout);
}

void Viewport2D::mouseMoveEvent(QMouseEvent *ev)
{
    if (mAxisMode == AxisMode::ORTHOGONAL)
        return;

    QPoint currentPoint = ev->pos();
    int dx = currentPoint.x()-lastPoint2D.x();
    int dy = currentPoint.y()-lastPoint2D.y();

    this->mRenderer->rotate(dx,dy);
    lastPoint2D = QPoint(ev->pos().x(),ev->pos().y());
    emit Mouse_pos();
}

void Viewport2D::mousePressEvent(QMouseEvent *ev)
{
#ifndef NDEBUG
    qDebug() << "Clicked in 2D viewport area";
#endif

    lastPoint2D = QPoint(ev->pos().x(),ev->pos().y());

    emit Mouse_pressed();
}

void Viewport2D::wheelEvent(QWheelEvent *ev)
{
    //qDebug() << "Mouse scroll in 2D viewport";
    const auto degrees = ev->angleDelta().y() / 8.0;
    double z = degrees/10.0;
    double speed = 1.0;

    this->mRenderer->zoom(z*speed);
    emit Mouse_scroll();
}

void Viewport2D::volumeSwitched() {
    mRenderer->mPrivateVolume = mVolume;
}

Viewport2D::~Viewport2D() = default;

void Viewport2D::load() {
    if (parentWidget()) {
        auto mainwindow = dynamic_cast<MainWindow*>(parentWidget()->parentWidget());
        if (mainwindow) {
            auto dataloader = mainwindow->loadData();
            connect(dataloader, &DataWidget::loaded, this, [&](std::shared_ptr<Volume> volume){
                mRenderer->mPrivateVolume = volume;
                mRemoveVolumeAction->setChecked(false);
                mRenderer->mUseGlobalVolume = false;
            });
        }
    }
}

void Viewport2D::removeVolume(bool bState) {
    // If user manually toggles it off, it should'nt do anything.
    if (!bState) {
        // Just enable the bool again. >:)
        mRemoveVolumeAction->setChecked(true);
        return;
    }

    mRenderer->mUseGlobalVolume = true;
    // Delete ptr by replacing it with nothing
    mRenderer->mPrivateVolume = {};
}

void Viewport2D::setAxis(QAction* axis) {
    axis->setChecked(true);
    
    // Unset all other axis:
    for (auto other : mAxisActions)
        if (other != axis)
            other->setChecked(false);

    // Set axis mode to button pressed
    if (axis == mAxisActions[3])
        mAxisMode = AxisMode::ARBITRARY;
    else if (axis == mAxisActions[4])
        mAxisMode = AxisMode::SLICE;
    else
        mAxisMode = AxisMode::ORTHOGONAL;


    // Reset view matrix to corresponding representation space:
    switch (mAxisMode) {
        case AxisMode::ORTHOGONAL:
            {
                const auto& end = mAxisActions.begin() + 3;
                const auto dirIt = std::find(mAxisActions.begin(), end, axis);
                if (dirIt != end) {
                    const auto i = dirIt - mAxisActions.begin();
                    auto& viewMat = mRenderer->getViewMatrix();
                    const auto zoom = viewMat(2,3);
                    viewMat.setToIdentity();
                    if (i == 1)
                        viewMat.rotate(90.f, {0.f, 1.f, 0.f});
                    else if (i == 2)
                        viewMat.rotate(-90.f, {1.f, 0.f, 0.f});
                    
                    viewMat.translate(0.f, 0.f, zoom);
                    mRenderer->viewMatrixUpdated();
                }
            }
            break;
        case AxisMode::ARBITRARY:
            break;
        case AxisMode::SLICE:
            {
                const auto& volume = mRenderer->getVolume();
                if (volume) {
                    const auto& plane = volume->m_slicingGeometry;
                    auto& viewMat = mRenderer->getViewMatrix();
                    viewMat.lookAt(plane.pos, plane.pos + plane.dir, {0.f, 1.f, 0.f});
                    mRenderer->viewMatrixUpdated();
                }
            }
            break;
    }

    mRenderer->mIsSlicePlaneEnabled = mRenderer->mIsCameraLinkedToSlicePlane = mAxisMode == AxisMode::SLICE;
}