#include "viewport2d.h"
#include "ui_viewport2d.h"
#include "renderer.h"

Viewport2D::Viewport2D(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewport2D)
{
    ui->setupUi(this);
    mRenderer = std::make_unique<Renderer>(this);
    ui->RenderLayout->addWidget(mRenderer.get());
}

Viewport2D::~Viewport2D()
{
    delete ui;
}
