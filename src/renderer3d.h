#ifndef RENDERER3D_H
#define RENDERER3D_H

#include "renderer.h"

class Renderer3D : public Renderer {
public:
    Renderer3D(QWidget *parent = nullptr) : Renderer{parent} {}

protected:
    void initializeGL() override;
    void paintGL() override;
};

#endif // RENDERER3D_H