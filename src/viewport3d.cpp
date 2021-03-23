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

void Viewport3D::mouseMoveEvent(QMouseEvent *ev)
{
    QPoint currentPoint = ev->pos();
    int dx = currentPoint.x()-lastPoint3D.x();
    int dy = currentPoint.y()-lastPoint3D.y();

    this->mRenderer->rotate(dx,dy);
    lastPoint3D = QPoint(ev->pos().x(),ev->pos().y());
    emit Mouse_pos3D();
}

void Viewport3D::mousePressEvent(QMouseEvent *ev)
{
    qDebug() << "Clicked in 3D viewport area";

    lastPoint3D = QPoint(ev->pos().x(),ev->pos().y());

    emit Mouse_pressed3D();
}

void Viewport3D::wheelEvent(QWheelEvent *ev)
{
    //qDebug() << "Mouse scroll in 3D viewport";
    const int degrees = ev->delta() / 8;
    double z = degrees/10;
    double speed = 1.0;

    this->mRenderer->zoom(z*speed);
    emit Mouse_scroll3D();
}


Viewport3D::~Viewport3D() = default;
