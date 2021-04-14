#include "viewport3d.h"
#include "renderer3d.h"
#include <QVBoxLayout>
#include "mainwindow.h"
#include "volume.h"
#include "datawidget.h"

Viewport3D::Viewport3D(QWidget *parent) :
    QWidget{parent}, IMenu{this}
{
    // Layout:
    mLayout = new QVBoxLayout{this};
    mLayout->setContentsMargins(0, 0, 0, 0);

    // Menubar:
    auto datamenu = mMenuBar->addMenu("Data");
    auto openAction = datamenu->addAction("Open");
    mRemoveVolumeAction = datamenu->addAction("Use global volume");
    mRemoveVolumeAction->setCheckable(true);
    mRemoveVolumeAction->setChecked(true);

    // Slice menu
    auto slicemenu = mMenuBar->addMenu("Slicing");
    auto sliceEnable = slicemenu->addAction("Enable");
    sliceEnable->setCheckable(true);
    auto sliceMove = slicemenu->addAction("Linked to camera");
    sliceMove->setCheckable(true);

    mLayout->addWidget(mMenuBar);

    // OpenGL Render Widget:
    mRenderer = new Renderer3D{this};
    mLayout->addWidget(mRenderer);

    // Connections:
    connect(openAction, &QAction::triggered, this, &Viewport3D::load);
    connect(mRemoveVolumeAction, &QAction::triggered, this, &Viewport3D::removeVolume);
    connect(sliceEnable, &QAction::toggled, this, [&](bool bEnabled){
        mRenderer->mIsSlicePlaneEnabled = bEnabled;
    });
    connect(sliceMove, &QAction::toggled, this, [&](bool bEnabled){
        mRenderer->mIsCameraLinkedToSlicePlane = bEnabled;
    });


    setLayout(mLayout);
}

void Viewport3D::mouseMoveEvent(QMouseEvent *ev)
{
    QPoint currentPoint = ev->pos();
    int dx = currentPoint.x()-lastPoint3D.x();
    int dy = currentPoint.y()-lastPoint3D.y();

    this->mRenderer->rotate(dx,dy);
    lastPoint3D = QPoint(ev->pos().x(),ev->pos().y());
    emit Mouse_pos3D();
}

void Viewport3D::mousePressEvent(QMouseEvent *ev)
{
#ifndef NDEBUG
    qDebug() << "Clicked in 3D viewport area";
#endif

    lastPoint3D = QPoint(ev->pos().x(),ev->pos().y());

    emit Mouse_pressed3D();
}

void Viewport3D::wheelEvent(QWheelEvent *ev)
{
    //qDebug() << "Mouse scroll in 3D viewport";
    const auto degrees = ev->angleDelta().y() / 8.0;
    double z = degrees/10.0;
    double speed = 1.0;

    this->mRenderer->zoom(z*speed);
    emit Mouse_scroll3D();
}

void Viewport3D::load() {
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

void Viewport3D::removeVolume(bool bState) {
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

Viewport3D::~Viewport3D() = default;