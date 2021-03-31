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

class NodeGlyphs : protected QOpenGLFunctions_4_5_Core {
private:
    GLuint mVAO, mVBO;
    GLsizei mNodeCount{0};

public:
    NodeGlyphs(const std::vector<QVector2D>& nodePos = {});

    void draw();
    void resizeNodeBuffer(const std::vector<QVector2D>& nodePos);

    ~NodeGlyphs();
};

#endif // RENDERUTILS_H