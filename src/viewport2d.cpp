#include "viewport2d.h"
#include "renderer.h"
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
    mLayout->addWidget(mMenuBar);

    // OpenGL Render Widget:
    mRenderer = new Renderer{this};
    mLayout->addWidget(mRenderer);

    setLayout(mLayout);
}

Viewport2D::~Viewport2D() = default;

void Viewport2D::load() {
    if (parentWidget()) {
        auto mainwindow = dynamic_cast<MainWindow*>(parentWidget()->parentWidget());
        if (mainwindow) {
            mRenderer->mUseGlobalVolume = true;
            mRenderer->mPrivateVolume = std::make_shared<Volume>();
            mainwindow->loadData(mRenderer->mPrivateVolume.get());
        }
    }
}