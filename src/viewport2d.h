#ifndef VIEWPORT2D_H
#define VIEWPORT2D_H

#include <QWidget>

#include "menuinterface.h"

class Renderer;
class QVBoxLayout;
class QAction;

class Viewport2D : public QWidget, public IMenu
{
    Q_OBJECT

public:
    explicit Viewport2D(QWidget *parent = nullptr);
    ~Viewport2D();

private:
    Renderer* mRenderer{nullptr};
    QVBoxLayout* mLayout{nullptr};

private slots:
    void load();

};

#endif // VIEWPORT2D_H
