#ifndef RENDERER3D_H
#define RENDERER3D_H

#include "renderer.h"

class Renderer2D : public Renderer {
public:
    Renderer2D(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;
};

#endif // RENDERER3D_H