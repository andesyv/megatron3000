#ifndef RENDERER_H
#define RENDERER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include "shaders/shadermanager.h"
#include <QMatrix4x4>
#include <QElapsedTimer>
#include <memory>

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

class MainWindow;
class Volume;

class Renderer : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
public:
    Renderer(QWidget *parent = nullptr);

    double getFramesPerSecond();

    // Whether to use widgets camera or global camera
    bool mUseGlobalMatrix{false};
    // Render-widgets own camera matrix. If not using global camera matrix, render-widgets use this one.
    QMatrix4x4 mPrivateViewMatrix;

    // Whether to use widgets volume data or global volume data
    bool mUseGlobalVolume{true};
    std::shared_ptr<Volume> mPrivateVolume;

    ~Renderer();

    virtual void zoom(double z);
    void rotate(float dx, float dy);

    virtual const QMatrix4x4& getViewMatrix() const; 
    virtual std::shared_ptr<Volume> getVolume() const;
    virtual QMatrix4x4& getViewMatrix(); 
    virtual std::shared_ptr<Volume> getVolume();
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;


    MainWindow* mMainWindow;

    // Screen Spaced vertex array
    std::unique_ptr<ScreenSpacedBuffer> mScreenVAO;

    QMatrix4x4 mPerspectiveMatrix;

    QElapsedTimer mFrameTimer, mAliveTimer;
    uint32_t mFrameCount{0};

    // Shader interface:
    Shader& shaderProgram(const std::string& name);
    bool isShaderValid(const std::string& name) const;

private slots:
    void scheduleRender();
};

#endif // RENDERER_H
