#ifndef RENDERUTILS_H
#define RENDERUTILS_H

#include <QOpenGLFunctions_4_5_Core>
#include <QVector2D>
#include <QMatrix4x4>

/**
 * @brief Helper class to draw a screen spaced quad
 */
class ScreenSpacedBuffer : protected QOpenGLFunctions_4_5_Core {
private:
    GLuint mVAO, mVBO;

public:
    void bind();
    void unbind();

    void draw();

    ScreenSpacedBuffer();
    ~ScreenSpacedBuffer();
};

/**
 * @brief Helper class for drawing an axis helper visualizer
 */
class AxisGlyph : protected QOpenGLFunctions_4_5_Core {
private:
    GLuint mVAO, mVBO;

public:
    void bind();
    void unbind();

    void draw();

    AxisGlyph();
    ~AxisGlyph();
};

/**
 * @brief Hepler class to draw a bunch of nodes
 * Takes a bunch of normalized coordinates and draws them as points
 * which is then transformed into spheres on the GPU.
 */
class NodeGlyphs : protected QOpenGLFunctions_4_5_Core {
private:
    GLuint mVAO, mVBO;
    GLsizei mNodeCount{0};

public:
    NodeGlyphs(const std::vector<QVector2D>& nodePos = {});

    void draw(float aspectRatio = 1.f, float radius = 0.1, int selectedNode = -1);
    void resizeNodeBuffer(const std::vector<QVector2D>& nodePos);
    void updateNodeBuffer(const std::vector<QVector2D>& nodePos);

    ~NodeGlyphs();
};


constexpr float MatIdentityValues[] = {1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f};

class WorldPlaneGlyph : protected QOpenGLFunctions_4_5_Core {
private:
    GLuint mVAO, mVBO;

public:
    void bind();
    void unbind();

    GLfloat mAlpha = 0.05f;

    void draw(const QMatrix4x4& MVP = QMatrix4x4{MatIdentityValues}, const QVector3D& up = {0.f, 1.f, 0.f},
              const QVector3D& pos = {0.f, 0.f, 0.f}, const QVector3D& dir = {0.f, 0.f, -1.f});

    WorldPlaneGlyph();
    ~WorldPlaneGlyph();
};


class LightGlobeGlyph : protected QOpenGLFunctions_4_5_Core {
private:
    GLuint mVAO, mVBO;

public:
    void bind();
    void unbind();

    LightGlobeGlyph(const QVector2D& pos = {});
    ~LightGlobeGlyph();
};



inline QVector2D aspectScale(float aspectRatio = 1.f) {
    return {1.0 < aspectRatio ? 1.f / aspectRatio : 1.f, 1.0 < aspectRatio ? 1.f : aspectRatio};
}

QVector2D screenPointToNormalizedCoordinates(const QPoint& point, int width, int height);

enum class HistogramSource : uint8_t {
    All = 0,
    Plane
};

#endif // RENDERUTILS_H