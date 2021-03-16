#include "viewport3d.h"
#include "renderer3d.h"
#include <QVBoxLayout>

Viewport3D::Viewport3D(QWidget *parent) :
    QWidget{parent}, IMenu{this}
{
    // Layout:
    mLayout = new QVBoxLayout{this};
    mLayout->setContentsMargins(0, 0, 0, 0);

    // Menubar:
    auto datamenu = mMenuBar->addMenu("Data");
    datamenu->addAction("Load");
    mLayout->addWidget(mMenuBar);

    // OpenGL Render Widget:
    mRenderer = new Renderer3D{this};
    mLayout->addWidget(mRenderer);

    setLayout(mLayout);
}

Viewport3D::~Viewport3D() = default;