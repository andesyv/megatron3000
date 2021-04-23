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

#ifndef EMBEDDED_SHADERS
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
#endif

    mPrivateViewMatrix.setToIdentity();
    mPrivateViewMatrix.translate({0.f, 0.f, -4.f});
}

void Renderer3D::paintGL() {
    const float deltaTime = mFrameTimer.restart() * 0.001;

    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // mPrivateViewMatrix.rotate(10.0f * deltaTime, QVector3D{0.5f, 1.f, 0.f});
    const auto& viewMatrix = getViewMatrix();
    QVector3D viewUp;
    const auto& volume = getVolume();
    Volume::Guard volumeGuard;

    auto& shader = shaderProgram("volume");
#ifndef NDEBUG
    if (!shader.isLinked()) return;
#endif
    
    if (volume)
        viewUp = (mViewMatrixInverse * QVector4D{0.f, 1.f, 0.f, 0.f}).toVector3D().normalized();
    /* I'm rendering one front-facing plane and one back-facing plane
     * in order to "simulate" the effect of a transparent two-sided
     * on both sides of the volume rendering. The first one takes
     * care of plane -> volume blending, and the second one takes
     * care of volume -> plane blending.
     */
    // Render front plane glyph
    if (volume && mIsSlicePlaneEnabled) {
        const auto& plane = volume->m_slicingGeometry;
        mPlane->draw(mPerspectiveMatrix * viewMatrix, viewUp, plane.pos, plane.dir);
    }

    shader.bind();
    shader.setUniformValue("MVP", mMVPInverse);
    
    if (volume) {
        shader.setUniformValue("volumeScale", volume->volumeScale());
        shader.setUniformValue("volumeSpacing", volume->volumeSpacing());
        shader.setUniformValue("isSlicingEnabled", mIsSlicePlaneEnabled);
        shader.setUniformValue("time", mAliveTimer.elapsed() * 0.001f);

        // Volume guard automatically binds and unbinds. :)
        volumeGuard = volume->guard();
    }

    glDisable(GL_CULL_FACE);
    mScreenVAO->draw();

    // Render back plane glyph
    if (volume && mIsSlicePlaneEnabled) {
        glFrontFace(GL_CW); // Reverse winding order to reverse plane direction
        const auto& plane = volume->m_slicingGeometry;
        mPlane->draw(mPerspectiveMatrix * viewMatrix, viewUp, plane.pos, plane.dir);
        glFrontFace(GL_CCW);
    }

    drawAxis();

    ++mFrameCount;
}

Renderer3D::~Renderer3D() {};