#ifndef RENDERUTILS_H
#define RENDERUTILS_H

#include <QOpenGLFunctions_4_5_Core>

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

    void draw(float radius = 0.1);
    void resizeNodeBuffer(const std::vector<QVector2D>& nodePos);
    void updateNodeBuffer(const std::vector<QVector2D>& nodePos);

    ~NodeGlyphs();
};

#endif // RENDERUTILS_H