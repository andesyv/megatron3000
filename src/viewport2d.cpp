#include "viewport2d.h"
#include "ui_viewport2d.h"

Viewport2D::Viewport2D(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewport2D)
{
    ui->setupUi(this);
}

Viewport2D::~Viewport2D()
{
    delete ui;
}
