#ifndef RENDERER_H
#define RENDERER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>
#include <QMatrix>
#include <QElapsedTimer>

class Renderer : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
    Q_OBJECT
public:
    Renderer();

    double getFramesPerSecond();

    ~Renderer();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    // Temporary opengl test stuff:
    GLuint mVAO, mVBO;
    QMatrix4x4 mMat;

    QOpenGLShaderProgram mShader;

    QElapsedTimer mFrameTimer, mAliveTimer;
    uint32_t mFrameCount{0};

private slots:
    void scheduleRender();
};

#endif // RENDERER_H
