#ifndef RENDERER_H
#define RENDERER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QMatrix4x4>
#include <QElapsedTimer>
#include <memory>

class MainWindow;
class Volume;
class ScreenSpacedBuffer;
class AxisGlyph;
class Shader;
class WorldPlaneGlyph;
class LightGlobeGlyph;

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

    std::shared_ptr<Volume> mVolume;

    bool mIsSlicePlaneEnabled{false};
    bool mIsCameraLinkedToSlicePlane{false};

    std::unique_ptr<WorldPlaneGlyph> mPlane;

    ~Renderer() override;

    virtual void zoom(double z);
    virtual void rotate(float dx, float dy);

    virtual const QMatrix4x4& getViewMatrix() const; 
    virtual std::shared_ptr<Volume> getVolume() const;
    virtual QMatrix4x4& getViewMatrix(); 
    virtual std::shared_ptr<Volume> getVolume();

    void viewMatrixUpdated();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    // Helper function to render an axis on screen
    void drawAxis();


    MainWindow* mMainWindow;

    // Screen Spaced vertex array
    std::unique_ptr<ScreenSpacedBuffer> mScreenVAO;
    std::unique_ptr<AxisGlyph> mAxisGlyph;
    std::unique_ptr<LightGlobeGlyph> mLightGlobe;

    QMatrix4x4 mPerspectiveMatrix;

    // Cached inverse of perspective and MVP matrix for better performance
    QMatrix4x4 mViewMatrixInverse;
    QMatrix4x4 mMVPInverse;

    QElapsedTimer mFrameTimer, mAliveTimer;
    uint32_t mFrameCount{0};

    float mAspectRatio{1.f};

    // Shader interface:
    Shader& shaderProgram(const std::string& name);
    bool isShaderValid(const std::string& name) const;

private slots:
    void scheduleRender();
};

#endif // RENDERER_H
