#include "renderer3d.h"
#include "mainwindow.h"
#include "volume.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

Renderer3D::Renderer3D(QWidget *parent)
    : Renderer{parent} {
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
}

void Renderer3D::paintGL() {
    const float deltaTime = mFrameTimer.restart() * 0.001;

    glClear(GL_COLOR_BUFFER_BIT);

    mPrivateViewMatrix.rotate(10.0f * deltaTime, QVector3D{0.5f, 1.f, 0.f});
    const auto& viewMatrix = mUseGlobalMatrix ? mMainWindow->mGlobalViewMatrix : mPrivateViewMatrix;
    const auto MVP = (mPerspectiveMatrix * viewMatrix).inverted();

    auto& shader = shaderProgram("volume");
    if (!shader.isLinked()) return;

    shader.bind();
    shader.setUniformValue("MVP", MVP);

    // Volume guard automatically binds and unbinds. :)
    const auto volumeGuard = mMainWindow->mGlobalVolume->guard(0);

    mScreenVAO->draw();

    ++mFrameCount;
}