#ifndef VIEWPORT3D_H
#define VIEWPORT3D_H

#include <QWidget>

#include "menuinterface.h"

class Renderer3D;
class QVBoxLayout;

class Viewport3D : public QWidget, public IMenu
{
    Q_OBJECT

public:
    explicit Viewport3D(QWidget *parent = nullptr);
    ~Viewport3D();

private:
    Renderer3D* mRenderer{nullptr};
    QVBoxLayout* mLayout{nullptr};
};

#endif // VIEWPORT3D_H
