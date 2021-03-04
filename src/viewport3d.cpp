#include "viewport3d.h"
#include "ui_viewport3d.h"

Viewport3D::Viewport3D(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewport3D)
{
    ui->setupUi(this);
}

Viewport3D::~Viewport3D()
{
    delete ui;
}
