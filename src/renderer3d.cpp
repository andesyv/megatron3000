#include "renderer3d.h"
#include "mainwindow.h"
#include "volume.h"
#include <filesystem>
#include <iostream>
#include "renderutils.h"

namespace fs = std::filesystem;

void Renderer3D::initializeGL() {
    Renderer::initializeGL();

    // Create volume shader
    if (!isShaderValid("volume")) {
        auto& shader = shaderProgram("volume");

        const auto shaderpath = fs::absolute(fs::path{SHADERPATH});
        
        const auto vspath = QString::fromStdString((shaderpath / "screen.vs").string());
        const auto fspath = QString::fromStdString((shaderpath / "volume.fs").string());
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
    mPrivateViewMatrix.translate({0.f, 0.f, -4.f});
}

void Renderer3D::paintGL() {
    const float deltaTime = mFrameTimer.restart() * 0.001;

    glClear(GL_COLOR_BUFFER_BIT);

    // mPrivateViewMatrix.rotate(10.0f * deltaTime, QVector3D{0.5f, 1.f, 0.f});
    const auto& viewMatrix = getViewMatrix();
    const auto MVP = (mPerspectiveMatrix * viewMatrix).inverted();
    const auto& volume = getVolume();

    auto& shader = shaderProgram("volume");
#ifndef NDEBUG
    if (!shader.isLinked()) return;
#endif

    shader.bind();
    shader.setUniformValue("MVP", MVP);
    shader.setUniformValue("volumeScale", volume->volumeScale());
    shader.setUniformValue("volumeSpacing", volume->volumeSpacing());

    // Volume guard automatically binds and unbinds. :)
    const auto volumeGuard = volume->guard(0);

    mScreenVAO->draw();

    drawAxis();

    ++mFrameCount;
}
