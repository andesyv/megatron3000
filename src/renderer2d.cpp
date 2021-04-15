#include "renderer2d.h"
#include "mainwindow.h"
#include "volume.h"
#include <filesystem>
#include "renderutils.h"
#include "shaders/shadermanager.h"
#include <QDebug>

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
        if (!shader.addSourceRelative(QOpenGLShader::Vertex, "screen.vs")) {
            throw std::runtime_error{"Failed to compile vertex shader"};
        }

        if (!shader.addSourceRelative(QOpenGLShader::Fragment, "slice-image.fs")) {
            throw std::runtime_error{"Failed to compile fragment shader"};
        }

        if (!shader.link()) {
            // throw std::runtime_error{"Failed to link shaderprogram"};
            qDebug() << "Failed to link shader!";
        }
    }

    mPrivateViewMatrix.setToIdentity();
}

void Renderer2D::paintGL() {
    // const float deltaTime = mFrameTimer.restart() * 0.001;

    glClear(GL_COLOR_BUFFER_BIT);

    const auto& viewMatrix = getViewMatrix();
    const auto& volume = getVolume();
    Volume::Guard volumeGuard;
    const auto time = mFrameTimer.elapsed() * 0.001f;

    auto& shader = shaderProgram("slice");
#ifndef NDEBUG
    if (!shader.isLinked()) return;
#endif

    shader.bind();
    shader.setUniformValue("MVP", mMVPInverse);
    shader.setUniformValue("time", time);
    
    if (volume) {
        shader.setUniformValue("volumeScale", volume->volumeScale());
        shader.setUniformValue("volumeSpacing", volume->volumeSpacing());

        // Volume guard automatically binds and unbinds. :)
        volumeGuard = volume->guard();
    }
    
    mScreenVAO->draw();

    drawAxis();

    ++mFrameCount;
}

void Renderer2D::resizeGL(int w, int h) {
    mAspectRatio = static_cast<float>(w) / h;
    mPerspectiveMatrix.setToIdentity();
    mPerspectiveMatrix.ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
}