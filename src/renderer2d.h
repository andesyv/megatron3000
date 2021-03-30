#ifndef RENDERER3D_H
#define RENDERER3D_H

#include "renderer.h"

class Renderer2D : public Renderer {
public:
    Renderer2D(QWidget *parent = nullptr) : Renderer{parent} {}

    void zoom(double z) override;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
};

#endif // RENDERER3D_H