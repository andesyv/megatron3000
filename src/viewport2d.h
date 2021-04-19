#ifndef VIEWPORT2D_H
#define VIEWPORT2D_H

#include <QWidget>
#include <QMouseEvent>
#include <QWheelEvent>

#include "menuinterface.h"

class Renderer2D;
class QVBoxLayout;
class QAction;

class Viewport2D : public QWidget, public IMenu
{
    Q_OBJECT

public:
    enum AxisMode : unsigned int {
        ORTHOGONAL = 0, // Oriented in one of the orthogonal axis (x, y, z)
        ARBITRARY,      // Oriented in arbitrary axis desided by camera rotation
        SLICE           // Oriented in same axis as the cutting slice plane
    };

    explicit Viewport2D(QWidget *parent = nullptr);
    ~Viewport2D();
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);

signals:
    void Mouse_pressed();
    void Mouse_pos();
    void Mouse_scroll();

protected:
    void volumeSwitched() final;

private:
    Renderer2D* mRenderer{nullptr};
    QVBoxLayout* mLayout{nullptr};
    QPoint lastPoint2D;
    QAction* mRemoveVolumeAction{nullptr};
    std::vector<QAction*> mAxisActions;
    AxisMode mAxisMode{AxisMode::ORTHOGONAL};

private slots:
    void load();
    void removeVolume(bool bState);
    void setAxis(QAction* axis);

};

#endif // VIEWPORT2D_H
