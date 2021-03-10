#ifndef RENDERER_H
#define RENDERER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QElapsedTimer>
#include <memory>

/**
 * @brief Helper class to draw a screen spaced quad
 */
class ScreenSpacedBuffer : protected QOpenGLFunctions_4_3_Core {
private:
    GLuint mVAO, mVBO;

public:
    void bind();
    void unbind();

    void draw();

    ScreenSpacedBuffer();
    ~ScreenSpacedBuffer();
};

class MainWindow;

class Renderer : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
    Q_OBJECT
public:
    Renderer(QWidget *parent = nullptr);

    double getFramesPerSecond();

    ~Renderer();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    MainWindow* mMainWindow;

    // Screen Spaced vertex array
    std::unique_ptr<ScreenSpacedBuffer> mScreenVAO;
    // Render-widgets own camera matrix. If not using global camera matrix, render-widgets use this one.
    QMatrix4x4 mPrivateViewMatrix;
    // Whether to use widgets camera or global camera
    bool mUseGlobalMatrix{false};

    QElapsedTimer mFrameTimer, mAliveTimer;
    uint32_t mFrameCount{0};

    // Shader interface:
    QOpenGLShaderProgram& shaderProgram(const std::string& name);
    bool isShaderValid(const std::string& name) const;

private slots:
    void scheduleRender();
};

#endif // RENDERER_H
