#include "viewport2d.h"
#include "renderer2d.h"
#include <iostream>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QSizePolicy>
#include <QDockWidget>
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

Viewport2D::~Viewport2D() = default;

void Viewport2D::load() {
    if (parentWidget()) {
        auto mainwindow = dynamic_cast<MainWindow*>(parentWidget()->parentWidget());
        if (mainwindow) {
            mRenderer->mPrivateVolume = std::make_shared<Volume>();
            mainwindow->loadData(mRenderer->mPrivateVolume.get());
            connect(mRenderer->mPrivateVolume.get(), &Volume::loaded, this, [&](){
                mRemoveVolumeAction->setChecked(false);
                mRenderer->mUseGlobalVolume = true;
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