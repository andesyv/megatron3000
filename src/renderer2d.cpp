#include "renderer2d.h"
#include "mainwindow.h"
#include "volume.h"
#include <filesystem>
#include <iostream>
#include "renderutils.h"

namespace fs = std::filesystem;

void Renderer2D::zoom(double z) {
    auto& trans = mPrivateViewMatrix(2,3);
    trans = std::clamp(trans + static_cast<float>(z) * 0.02f, -1.f, 1.f);
}

void Renderer2D::initializeGL() {
    Renderer::initializeGL();

    // Create volume shader
    if (!isShaderValid("slice")) {
        auto& shader = shaderProgram("slice");

        const auto shaderpath = fs::absolute(fs::path{SHADERPATH});
        
        const auto vspath = QString::fromStdString((shaderpath / "screen.vs").string());
        const auto fspath = QString::fromStdString((shaderpath / "slice-image.fs").string());
        if (!shader.addSource(QOpenGLShader::Vertex, vspath)) {
            throw std::runtime_error{"Failed to compile vertex shader"};
        }

        if (!shader.addSource(QOpenGLShader::Fragment, fspath)) {
            throw std::runtime_error{"Failed to compile fragment shader"};
        }

        if (!shader.link()) {
            // throw std::runtime_error{"Failed to link shaderprogram"};
            std::cout << "Failed to link shader!" << std::endl;
        }
    }

    mPrivateViewMatrix.setToIdentity();
}

void Renderer2D::paintGL() {
    // const float deltaTime = mFrameTimer.restart() * 0.001;

    glClear(GL_COLOR_BUFFER_BIT);

    const auto& viewMatrix = getViewMatrix();
    const auto MVP = (mPerspectiveMatrix * viewMatrix).inverted();
    const auto& volume = getVolume();
    const auto time = mFrameTimer.elapsed() * 0.001f;

    auto& shader = shaderProgram("slice");
#ifndef NDEBUG
    if (!shader.isLinked()) return;
#endif

    shader.bind();
    shader.setUniformValue("MVP", MVP);
    shader.setUniformValue("volumeScale", volume->volumeScale());
    shader.setUniformValue("volumeSpacing", volume->volumeSpacing());
    shader.setUniformValue("time", time);

    // Volume guard automatically binds and unbinds. :)
    const auto volumeGuard = volume->guard(0);

    mScreenVAO->draw();

    drawAxis();

    ++mFrameCount;
}

void Renderer2D::resizeGL(int w, int h) {
    mAspectRatio = static_cast<float>(w) / h;
    mPerspectiveMatrix.setToIdentity();
    mPerspectiveMatrix.ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
}