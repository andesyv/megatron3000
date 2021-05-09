#ifndef RENDERER3D_H
#define RENDERER3D_H

namespace Slicing {
    struct Plane;
}

#include "renderer.h"
#include <memory>
#include <QQuaternion>
#include <QVector2D>

struct LightGlobe {
    float mRadius{0.12f};
    QVector2D mOffset{-1.f, -3.f};
    QQuaternion mRotation{1.f, 0.f, 0.f, 0.f};
    QVector2D mRotationSpeed{0.f, 0.f};
    const float mRotationFalloff = 3.f;

    void rotate(float dx, float fy);
    QVector3D dir() const;
};

class Renderer3D : public Renderer {
public:
    Renderer3D(QWidget *parent = nullptr) : Renderer{parent} {}
    ~Renderer3D() override;

    LightGlobe mGlobe;
    QVector2D globePosition() const;
    bool isGlobeIntersecting(const QVector2D& p) const;

    void rotate(float dx, float dy) override;

protected:
    void initializeGL() override;
    void paintGL() override;

    void drawGlobe();
};

#endif // RENDERER3D_H