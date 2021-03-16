#include "renderer.h"
#include <iostream>
#include <QElapsedTimer>
#include "shaders/shadermanager.h"
#include "mainwindow.h"

ScreenSpacedBuffer::ScreenSpacedBuffer() {
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // Pos (x,y)
    const GLfloat vertices[] = {
        -1.f, -1.f,
        1.f, -1.f,
        1.f, 1.f,

        -1.f, -1.f,
        1.f, 1.f,
        -1.f, 1.f
    };

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void ScreenSpacedBuffer::bind() {
    glBindVertexArray(mVAO);
}

void ScreenSpacedBuffer::unbind() {
    glBindVertexArray(0);
}

void ScreenSpacedBuffer::draw() {
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

ScreenSpacedBuffer::~ScreenSpacedBuffer() {
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}




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

    mScreenVAO = std::make_unique<ScreenSpacedBuffer>();

    glClearColor(0.f, 0.3f, 0.3f, 1.f);

    mPrivateViewMatrix.setToIdentity();
    mPrivateViewMatrix.translate({0.f, 0.f, -10.f});

    // Check if we could find MainWindow in contructor
    if (mMainWindow == nullptr) throw std::runtime_error{"Renderwidget could'nt find MainWindow!"};
}

void Renderer::paintGL() {
    const float deltaTime = mFrameTimer.restart() * 0.001;

    glClear(GL_COLOR_BUFFER_BIT);

    mPrivateViewMatrix.rotate(10.0f * deltaTime, QVector3D{0.5f, 1.f, 0.f});
    const auto& viewMatrix = mUseGlobalMatrix ? mMainWindow->mGlobalViewMatrix : mPrivateViewMatrix;
    const auto MVP = (mPerspectiveMatrix * viewMatrix).inverted();

    auto& shader = shaderProgram("screen");
    shader.bind();
    shader.setUniformValue("MVP", MVP);

    // Volume guard automatically binds and unbinds. :)
    const auto volumeGuard = mMainWindow->mGlobalVolume->guard(1);

    mScreenVAO->draw();

    ++mFrameCount;
}

void Renderer::resizeGL(int w, int h) {
    const auto aspectRatio = static_cast<float>(w) / h;
    mPerspectiveMatrix.setToIdentity();
    mPerspectiveMatrix.perspective(45.f, aspectRatio, 0.1f, 1000.f);
}

void Renderer::scheduleRender() {
    update();
}

Renderer::~Renderer() {}

QOpenGLShaderProgram& Renderer::shaderProgram(const std::string& name) {
    return ShaderManager::get().shader(name);
}

bool Renderer::isShaderValid(const std::string& name) const {
    return ShaderManager::get().valid(name);
}