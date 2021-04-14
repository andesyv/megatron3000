#include "renderer.h"
#include <iostream>
#include <QElapsedTimer>
#include "shaders/shadermanager.h"
#include "mainwindow.h"
#include "volume.h"
#include "renderutils.h"

Renderer::Renderer(QWidget *parent)
    : QOpenGLWidget{parent} {
    /* Use the formats bufferswap to determine when to redraw the widget.
     * After each bufferswap, determined by swap interval in the OpenGL context,
     * a new render is scheduled. This limits the amount of rendering done to
     * be no more than the refresh rate of the screen, preventing wasted frames.
     */
    connect(this, &QOpenGLWidget::frameSwapped, this, &Renderer::scheduleRender);

    // Follow outer chain to find main window:
    auto widget = window();
    while (widget != nullptr && !widget->isWindow())
        widget = widget->window();
    
    mMainWindow = dynamic_cast<MainWindow*>(widget);
}

double Renderer::getFramesPerSecond() {
    const auto elapsed = mAliveTimer.elapsed() * 0.001;
    return 1.0 / (elapsed / mFrameCount);
}

void Renderer::initializeGL() {
    initializeOpenGLFunctions();

#ifndef NDEBUG
    // OpenGL Debugger callback:
    static auto debugMessageCallback = [](
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar *message,
            const void *userParam) {
        std::cout << "GL DEBUG: " << message << std::endl;
    };
    glDebugMessageCallback(debugMessageCallback, nullptr);
#endif

    mScreenVAO = std::make_unique<ScreenSpacedBuffer>();
    mAxisGlyph = std::make_unique<AxisGlyph>();
    mPlane = std::make_unique<WorldPlaneGlyph>();

    glClearColor(0.f, 0.3f, 0.3f, 1.f);

    mPrivateViewMatrix.setToIdentity();
    mPrivateViewMatrix.translate({0.f, 0.f, -10.f});

    // Check if we could find MainWindow in contructor
    if (mMainWindow == nullptr) throw std::runtime_error{"Renderwidget could'nt find MainWindow!"};
}

void Renderer::paintGL() {
    const float deltaTime = mFrameTimer.restart() * 0.001;

    glClear(GL_COLOR_BUFFER_BIT);

    // mPrivateViewMatrix.rotate(10.0f * deltaTime, QVector3D{0.5f, 1.f, 0.f});
    const auto& viewMatrix = getViewMatrix();
    const auto MVP = (mPerspectiveMatrix * viewMatrix).inverted();
    const auto& volume = getVolume();

    auto& shader = shaderProgram("screen");
    shader.bind();
    shader.setUniformValue("MVP", MVP);

    // Volume guard automatically binds and unbinds. :)
    const auto volumeGuard = volume->guard(0);

    mScreenVAO->draw();

    drawAxis();

    ++mFrameCount;
}

void Renderer::resizeGL(int w, int h) {
    mAspectRatio = static_cast<float>(w) / h;
    mPerspectiveMatrix.setToIdentity();
    mPerspectiveMatrix.perspective(45.f, mAspectRatio, 0.1f, 1000.f);
    // mPerspectiveMatrix.ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
}

const QMatrix4x4& Renderer::getViewMatrix() const {
    return mUseGlobalMatrix ? mMainWindow->mGlobalViewMatrix : mPrivateViewMatrix;
}

std::shared_ptr<Volume> Renderer::getVolume() const {
    return mUseGlobalVolume ? mMainWindow->mGlobalVolume : mPrivateVolume;
}

QMatrix4x4& Renderer::getViewMatrix() {
    return mUseGlobalMatrix ? mMainWindow->mGlobalViewMatrix : mPrivateViewMatrix;
}

std::shared_ptr<Volume> Renderer::getVolume() {
    return mUseGlobalVolume ? mMainWindow->mGlobalVolume : mPrivateVolume;
}

void Renderer::drawAxis() {
    const auto& viewMatrix = getViewMatrix();
    const QVector2D aspectRatio = {1.0 < mAspectRatio ? 1.f / mAspectRatio : 1.f, 1.0 < mAspectRatio ? 1.f : mAspectRatio};

    auto& shader = shaderProgram("axis");
    if (!shader.isLinked()) return;
    shader.bind();
    shader.setUniformValue("viewMatrix", viewMatrix);
    shader.setUniformValue("aspectRatio", aspectRatio);

    mAxisGlyph->draw();
}

void Renderer::scheduleRender() {
    update();
}

Renderer::~Renderer() {}

void Renderer::zoom(double z)
{
#ifndef NDEBUG
    QMatrix4x4 pos = mPrivateViewMatrix;
    qDebug() << pos;
#endif
    mPrivateViewMatrix(2,3) += z/2;

    auto volume = getVolume();
    if (volume && mIsSlicePlaneEnabled && mIsCameraLinkedToSlicePlane) {
        const auto& dir = QVector3D{mPrivateViewMatrix * QVector4D{0.f, 0.f, 1.f, 0.f}};
        volume->m_slicingGeometry.pos += dir * (z / 2);
    }
}


void Renderer::rotate(float dx, float dy)
{  
    QVector3D rotVec = QVector3D(dy,dx,0.f);

    QMatrix4x4 inversePrivateView = mPrivateViewMatrix.inverted();
    QVector4D transformedAxis = inversePrivateView*QVector4D(rotVec,0.f);

    const auto rotation = QQuaternion::fromAxisAndAngle(transformedAxis.toVector3D(), 0.5f*rotVec.length());
    mPrivateViewMatrix.rotate(rotation);

    // Rotate plane:
    auto volume = getVolume();
    if (volume && mIsSlicePlaneEnabled && mIsCameraLinkedToSlicePlane) {
        // const auto model = volume->m_slicingGeometry.model();
        // const auto& newView = mPrivateViewMatrix.inverted();
        // const auto& a = inversePrivateView * QVector4D{0.f, 0.f, 1.f, 0.f};
        // const auto& b = newView * QVector4D{0.f, 0.f, 1.f, 0.f};
        // const auto rotation = QQuaternion::rotationTo(a.toVector3D(), b.toVector3D());
        volume->m_slicingGeometry.dir = (mPrivateViewMatrix.inverted() * QVector4D{0.f, 0.f, 1.f, 0.f}).toVector3D();
    }
}

Shader& Renderer::shaderProgram(const std::string& name) {
    return ShaderManager::get().shader(name);
}

bool Renderer::isShaderValid(const std::string& name) const {
    return ShaderManager::get().valid(name);
}
