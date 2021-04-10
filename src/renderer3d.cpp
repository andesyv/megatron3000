#include "renderer3d.h"
#include "mainwindow.h"
#include "volume.h"
#include <filesystem>
#include "renderutils.h"
#include "shaders/shadermanager.h"
#include <QDebug>

namespace fs = std::filesystem;
using namespace Slicing;

void Renderer3D::initializeGL() {
    Renderer::initializeGL();

    // Create volume shader
    if (!isShaderValid("volume")) {
        auto& shader = shaderProgram("volume");
        if (!shader.addSourceRelative(QOpenGLShader::Vertex, "screen.vs")) {
            throw std::runtime_error{"Failed to compile vertex shader"};
        }

        if (!shader.addSourceRelative(QOpenGLShader::Fragment, "volume.fs")) {
            throw std::runtime_error{"Failed to compile fragment shader"};
        }

        if (!shader.link()) {
            // throw std::runtime_error{"Failed to link shaderprogram"};
            qDebug() << "Failed to link shader!";
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
    Volume::Guard volumeGuard;

    auto& shader = shaderProgram("volume");
#ifndef NDEBUG
    if (!shader.isLinked()) return;
#endif

    shader.bind();
    shader.setUniformValue("MVP", MVP);
    
    if (volume) {
        shader.setUniformValue("volumeScale", volume->volumeScale());
        shader.setUniformValue("volumeSpacing", volume->volumeSpacing());
        shader.setUniformValue("isSlicingEnabled", true);

        // Transform slicing plane and update buffer:
        // auto slicingGeometry = transformSlicingGeometry(MVP, volume->m_slicingGeometry);
        // volume->updateSlicingGeometryBuffer(slicingGeometry);

        // Volume guard automatically binds and unbinds. :)
        volumeGuard = volume->guard();
    }

    mScreenVAO->draw();

    drawAxis();

    ++mFrameCount;
}

Plane Renderer3D::transformSlicingGeometry(const QMatrix4x4& trans, const Plane& geometry) const {
    auto tPos = trans * QVector4D{geometry.pos, 1.f};
    tPos *= tPos.w();
    auto tDir = trans * QVector4D{geometry.dir, 0.f};
    tDir *= tDir.w();
    tDir.normalize();

    return {QVector3D{tPos}, QVector3D{tDir}};
}