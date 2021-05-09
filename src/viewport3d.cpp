#include "viewport3d.h"
#include "renderer3d.h"
#include <QVBoxLayout>
#include "mainwindow.h"
#include "volume.h"
#include "datawidget.h"
#include <QWidgetAction>
#include <QSlider>
#include <QLabel>
#include "renderutils.h"
#include <QMenuBar>
#include <QShortcut>

Viewport3D::Viewport3D(QWidget *parent) :
    QWidget{parent}, IMenu{this}
{
    // Layout:
    mLayout = new QVBoxLayout{this};
    mLayout->setContentsMargins(0, 0, 0, 0);

    // Shortcuts:
    const auto sliceKeys = QKeySequence{tr("S", "Toggle slice")};
    const auto mvTglKeys = QKeySequence{tr("L", "Toggle linked to camera")};

    // Slice menu
    auto slicemenu = mMenuBar->addMenu("Slicing");
    auto sliceEnable = slicemenu->addAction("Enable");
    sliceEnable->setCheckable(true);
    // This shortcut will actually never be run because widget can never get focus,
    // but I want the shortcut to be there. (hacky solution, but you can't argue with the results)
    sliceEnable->setShortcut(sliceKeys);
    sliceEnable->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);

    auto sliceEnableProxyShortcut = new QShortcut{sliceKeys, parent};
    sliceEnableProxyShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
    connect(sliceEnableProxyShortcut, &QShortcut::activated, sliceEnable, &QAction::toggle);

    mSliceMoveToggle = slicemenu->addAction("Linked to camera");
    mSliceMoveToggle->setCheckable(true);
    // Again, just a fake shortcut:
    mSliceMoveToggle->setShortcut(mvTglKeys);
    mSliceMoveToggle->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);

    // Actual shortcut:
    auto sliceMoveToggleProxyShortcut = new QShortcut{mvTglKeys, parent};
    sliceMoveToggleProxyShortcut->setContext(Qt::ShortcutContext::WidgetWithChildrenShortcut);
    connect(sliceMoveToggleProxyShortcut, &QShortcut::activated, mSliceMoveToggle, &QAction::toggle);

    // Opacity slider menu widget:
    auto sliceSlider = new QWidgetAction{this};
    auto sliderWidgetWrapper = new QWidget{};
    auto sliderLayout = new QVBoxLayout{};
    sliderLayout->addWidget(new QLabel{"Plane opacity:"});
    auto slider = new QSlider{Qt::Horizontal};
    slider->setMinimum(0);
    slider->setMaximum(100);
    slider->setValue(5);
    sliderLayout->addWidget(slider);
    sliderWidgetWrapper->setLayout(sliderLayout);
    sliceSlider->setDefaultWidget(sliderWidgetWrapper);
    slicemenu->addAction(sliceSlider);

    mLayout->addWidget(mMenuBar);

    // OpenGL Render Widget:
    mRenderer = new Renderer3D{this};
    if (mVolume)
        mRenderer->mVolume = mVolume;
    mLayout->addWidget(mRenderer);

    // Connections:
    connect(sliceEnable, &QAction::toggled, this, [&](bool bEnabled){
        mRenderer->mIsSlicePlaneEnabled = bEnabled;
    });
    connect(mSliceMoveToggle, &QAction::toggled, this, [&](bool bEnabled){
        mRenderer->mIsCameraLinkedToSlicePlane = bEnabled;
    });
    connect(slider, &QAbstractSlider::valueChanged, this, [&](int value){
        const auto percentage = value * 0.01f;
        mRenderer->mPlane->mAlpha = percentage;
    });


    setLayout(mLayout);
}

void Viewport3D::mouseMoveEvent(QMouseEvent *ev)
{
    QPoint currentPoint = ev->pos();
    int dx = currentPoint.x()-lastPoint3D.x();
    int dy = currentPoint.y()-lastPoint3D.y();

    if (mMovingGlobe)
        mRenderer->mGlobe.mRotationSpeed += QVector2D{static_cast<float>(dx), static_cast<float>(dy)};
    else
        mRenderer->rotate(dx,dy);
    lastPoint3D = QPoint(ev->pos().x(),ev->pos().y());
    emit Mouse_pos3D();
}

void Viewport3D::mousePressEvent(QMouseEvent *ev)
{
#ifndef NDEBUG
    qDebug() << "Clicked in 3D viewport area";
#endif

    if (ev->button() == Qt::MouseButton::RightButton)
        if (mRenderer && mRenderer->mIsSlicePlaneEnabled && !mRenderer->mIsCameraLinkedToSlicePlane)
            mSliceMoveToggle->toggle();

    lastPoint3D = QPoint(ev->pos().x(),ev->pos().y());

    const auto renderMousePos = mRenderer->mapFromParent(lastPoint3D);
    const auto mousePosNormalizedCoordinates = screenPointToNormalizedCoordinates(renderMousePos, mRenderer->width(), mRenderer->height());
    if (mRenderer->isGlobeIntersecting(mousePosNormalizedCoordinates))
        mMovingGlobe = true;

    emit Mouse_pressed3D();
}

void Viewport3D::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::MouseButton::RightButton)
        if (mRenderer && mRenderer->mIsSlicePlaneEnabled && mRenderer->mIsCameraLinkedToSlicePlane)
            mSliceMoveToggle->toggle();

    mMovingGlobe = false;
}

void Viewport3D::wheelEvent(QWheelEvent *ev)
{
    //qDebug() << "Mouse scroll in 3D viewport";
    const auto degrees = ev->angleDelta().y() / 8.0;
    double z = degrees/10.0;
    double speed = 0.1;

    this->mRenderer->zoom(z*speed);
    emit Mouse_scroll3D();
}

void Viewport3D::volumeSwitched() {
    mRenderer->mVolume = mVolume;
}

Viewport3D::~Viewport3D() = default;