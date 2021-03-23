#ifndef VIEWPORT2D_H
#define VIEWPORT2D_H

#include <QWidget>

#include "menuinterface.h"

class Renderer2D;
class QVBoxLayout;

class Viewport2D : public QWidget, public IMenu
{
    Q_OBJECT

public:
    explicit Viewport2D(QWidget *parent = nullptr);
    ~Viewport2D();

private:
    Renderer2D* mRenderer{nullptr};
    QVBoxLayout* mLayout{nullptr};

};

#endif // VIEWPORT2D_H
