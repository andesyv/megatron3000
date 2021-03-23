#include "viewport3d.h"
#include "renderer3d.h"
#include <QVBoxLayout>
#include "mainwindow.h"
#include "volume.h"

Viewport3D::Viewport3D(QWidget *parent) :
    QWidget{parent}, IMenu{this}
{
    // Layout:
    mLayout = new QVBoxLayout{this};
    mLayout->setContentsMargins(0, 0, 0, 0);

    // Menubar:
    auto datamenu = mMenuBar->addMenu("Data");
    datamenu->addAction("Load");
    auto openAction = datamenu->addAction("Open");
    connect(openAction, &QAction::triggered, this, &Viewport3D::load);
    mRemoveVolumeAction = datamenu->addAction("Use global volume");
    mRemoveVolumeAction->setCheckable(true);
    mRemoveVolumeAction->setChecked(true);
    connect(mRemoveVolumeAction, &QAction::triggered, this, &Viewport3D::removeVolume);
    
    mLayout->addWidget(mMenuBar);

    // OpenGL Render Widget:
    mRenderer = new Renderer3D{this};
    mLayout->addWidget(mRenderer);

    setLayout(mLayout);
}

Viewport3D::~Viewport3D() = default;

void Viewport3D::load() {
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