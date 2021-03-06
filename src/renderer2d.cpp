#include "renderer2d.h"
#include "mainwindow.h"
#include "volume.h"
#include "renderutils.h"
#include "shaders/shadermanager.h"
#include <QDebug>

void Renderer2D::zoom(double z) {
    auto& view = getViewMatrix();

    auto& trans = view(2,3);
    const auto delta = std::clamp(trans + static_cast<float>(z) * 0.02f, -1.f, 1.f) - trans;
    trans += delta;
    viewMatrixUpdated();

    auto volume = getVolume();
    if (volume && mIsSlicePlaneEnabled && mIsCameraLinkedToSlicePlane) {
        auto pos = (mMVPInverse * QVector4D{0.f, 0.f, 0.f, 1.f}).toVector3D();
        volume->m_slicingGeometry.pos = pos;
        volume->updateSlicingGeometryBuffer();
    }
}

void Renderer2D::rotate(float dx, float dy) {
    Renderer::rotate(dx, dy);

    auto volume = getVolume();
    if (volume && mIsSlicePlaneEnabled && mIsCameraLinkedToSlicePlane) {
        volume->m_slicingGeometry.dir = (mMVPInverse * QVector4D{0.f, 0.f, -1.f, 0.f}).toVector3D().normalized();
        volume->m_slicingGeometry.pos = (mMVPInverse * QVector4D{0.f, 0.f, 0.f, 1.f}).toVector3D();

        mBlockSliceUpdate = true;
        volume->updateSlicingGeometryBuffer();
        mBlockSliceUpdate = false;
    }
}

void Renderer2D::updateViewMatrixFromSlicingPlane() {
    if (mBlockSliceUpdate)
        return;
        
    auto volume = getVolume();
    if (volume) {
        const auto& plane = volume->m_slicingGeometry;
        auto& view = getViewMatrix();
        view.setToIdentity();
        view.lookAt(plane.pos, plane.pos - plane.dir, {0.f, 1.f, 0.f});
        viewMatrixUpdated();
    }
}

void Renderer2D::initializeGL() {
    Renderer::initializeGL();

#ifndef EMBEDDED_SHADERS
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
#endif

    mPrivateViewMatrix.setToIdentity();
}

void Renderer2D::paintGL() {
    // const float deltaTime = mFrameTimer.restart() * 0.001;

    glClear(GL_COLOR_BUFFER_BIT);

    auto viewMatrix = getViewMatrix();
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