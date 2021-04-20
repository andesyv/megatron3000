#ifndef VIEWPORT3D_H
#define VIEWPORT3D_H

#include <QWidget>
#include <QMouseEvent>
#include <QWheelEvent>

#include "menuinterface.h"

class Renderer3D;
class QVBoxLayout;

class Viewport3D : public QWidget, public IMenu
{
    Q_OBJECT

public:
    explicit Viewport3D(QWidget *parent = nullptr);
    ~Viewport3D();
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void wheelEvent(QWheelEvent *ev) override;

signals:
    void Mouse_pressed3D();
    void Mouse_pos3D();
    void Mouse_scroll3D();

protected:
    void volumeSwitched() final;

private:
    Renderer3D* mRenderer{nullptr};
    QVBoxLayout* mLayout{nullptr};
    QPoint lastPoint3D;
    QAction* mRemoveVolumeAction{nullptr};
    QAction* mSliceMoveToggle{nullptr};
    
};

#endif // VIEWPORT3D_H
