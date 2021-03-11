#include "viewport3d.h"
#include "ui_viewport3d.h"
#include "renderer.h"

Viewport3D::Viewport3D(QWidget *parent) :
    QWidget{parent},
    ui{new Ui::Viewport3D}
{
    ui->setupUi(this);
    mRenderer = std::make_unique<Renderer>(this);
//    ui->RenderLayout->addWidget(mRenderer.get());
}

Viewport3D::~Viewport3D()
{
//    delete ui;
}
