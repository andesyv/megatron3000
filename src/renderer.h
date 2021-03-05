#ifndef RENDERER_H
#define RENDERER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>
#include <QMatrix>

class Renderer : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
    Q_OBJECT
public:
    Renderer();

    ~Renderer();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    // Temporary opengl test stuff:
    GLuint mVAO, mVBO;
    QMatrix4x4 mMat;

    QOpenGLShaderProgram mShader;
};

#endif // RENDERER_H
