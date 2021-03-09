#include "renderer.h"
#include <iostream>
#include <QElapsedTimer>
#include "shaders/shadermanager.h"

Renderer::Renderer() {
    /* Use the formats bufferswap to determine when to redraw the widget.
     * After each bufferswap, determined by swap interval in the OpenGL context,
     * a new render is scheduled. This limits the amount of rendering done to
     * be no more than the refresh rate of the screen, preventing wasted frames.
     */
   connect(this, &QOpenGLWidget::frameSwapped, this, &Renderer::scheduleRender);
}

double Renderer::getFramesPerSecond() {
    const auto elapsed = mAliveTimer.elapsed() * 0.001;
    return 1.0 / (elapsed / mFrameCount);
}

void Renderer::initializeGL() {
    initializeOpenGLFunctions();

    glClearColor(0.f, 0.3f, 0.3f, 1.f);

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // Pos (x,y,z), color (r,g,b)
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.f,    1.f, 0.f, 0.f,
        0.f, 0.5f, 0.f,    0.f, 1.f, 0.f,
        0.5f, -0.5f, 0.f,    0.f, 0.f, 1.f
    };

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);


    mMat.setToIdentity();
}

void Renderer::paintGL() {
    const float deltaTime = mFrameTimer.restart() * 0.001;

    glClear(GL_COLOR_BUFFER_BIT);

    mMat.rotate(0.1f, QVector3D{0.f, 1.f, 0.f});

    auto& shader = shaderProgram("default");
    shader.bind();
    shader.setUniformValue("MVP", mMat);

    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    ++mFrameCount;
}

void Renderer::resizeGL(int w, int h) {
    
}

void Renderer::scheduleRender() {
    update();
}

Renderer::~Renderer() {
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);
}

QOpenGLShaderProgram& Renderer::shaderProgram(const std::string& name) {
    return ShaderManager::get().shader(name);
}

bool Renderer::isShaderValid(const std::string& name) const {
    return ShaderManager::get().valid(name);
}