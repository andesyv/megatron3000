#include "renderer2d.h"
#include "mainwindow.h"
#include "volume.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

Renderer2D::Renderer2D(QWidget *parent)
    : Renderer{parent} {
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
}

void Renderer2D::initializeGL() {
    Renderer::initializeGL();

    mPrivateViewMatrix.setToIdentity();
}

void Renderer2D::paintGL() {
    const float deltaTime = mFrameTimer.restart() * 0.001;

    glClear(GL_COLOR_BUFFER_BIT);

    auto& shader = shaderProgram("slice");
    if (!shader.isLinked()) return;

    shader.bind();

    mScreenVAO->draw();

    ++mFrameCount;
}