#include "viewport2d.h"
#include "renderer2d.h"
#include <iostream>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QSizePolicy>
#include <QDockWidget>
#include <QDebug>
#include "mainwindow.h"
#include "volume.h"

Viewport2D::Viewport2D(QWidget *parent) :
    QWidget{parent}, IMenu{this}
{
    /// Apparantly it's easier to just make the ui from code..
    // ui->setupUi(this);

    // Layout:
    mLayout = new QVBoxLayout{this};
    mLayout->setContentsMargins(0, 0, 0, 0);

    // Menubar:
    auto datamenu = mMenuBar->addMenu("Data");
    auto openAction = datamenu->addAction("Open");
    connect(openAction, &QAction::triggered, this, &Viewport2D::load);
    mRemoveVolumeAction = datamenu->addAction("Use global volume");
    mRemoveVolumeAction->setCheckable(true);
    mRemoveVolumeAction->setChecked(true);
    connect(mRemoveVolumeAction, &QAction::triggered, this, &Viewport2D::removeVolume);
    
    mLayout->addWidget(mMenuBar);

    // OpenGL Render Widget:
    mRenderer = new Renderer2D{this};
    mLayout->addWidget(mRenderer);



    setLayout(mLayout);
}

void Viewport2D::mouseMoveEvent(QMouseEvent *ev)
{
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
    const int degrees = ev->angleDelta().y() / 8;
    double z = degrees/10;
    double speed = 1.0;

    this->mRenderer->zoom(z*speed);
    emit Mouse_scroll();
}

Viewport2D::~Viewport2D() = default;

void Viewport2D::load() {
    if (parentWidget()) {
        auto mainwindow = dynamic_cast<MainWindow*>(parentWidget()->parentWidget());
        if (mainwindow) {
            mRenderer->mPrivateVolume = std::make_shared<Volume>();
            mainwindow->loadData(mRenderer->mPrivateVolume.get());
            connect(mRenderer->mPrivateVolume.get(), &Volume::loaded, this, [&](){
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