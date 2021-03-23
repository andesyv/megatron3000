#ifndef VIEWPORT2D_H
#define VIEWPORT2D_H

#include <QWidget>
#include <QMouseEvent>
#include <QWheelEvent>

#include "menuinterface.h"

class Renderer;
class QVBoxLayout;

class Viewport2D : public QWidget, public IMenu
{
    Q_OBJECT

public:
    explicit Viewport2D(QWidget *parent = nullptr);
    ~Viewport2D();
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);

signals:
    void Mouse_pressed();
    void Mouse_pos();
    void Mouse_scroll();

private:
    Renderer* mRenderer{nullptr};
    QVBoxLayout* mLayout{nullptr};
    QPoint lastPoint2D;

};

#endif // VIEWPORT2D_H
