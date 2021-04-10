#ifndef RENDERER3D_H
#define RENDERER3D_H

namespace Slicing {
    struct Plane;
}

#include "renderer.h"

class Renderer3D : public Renderer {
public:
    Renderer3D(QWidget *parent = nullptr) : Renderer{parent} {}

protected:
    void initializeGL() override;
    void paintGL() override;

    Slicing::Plane transformSlicingGeometry(const QMatrix4x4& trans, const Slicing::Plane& geometry) const;
};

#endif // RENDERER3D_H