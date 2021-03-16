#include "renderer3d.h"
#include "mainwindow.h"
#include "volume.h"

Renderer3D::Renderer3D(QWidget *parent)
    : Renderer{parent} {
}

void Renderer3D::paintGL() {
    const float deltaTime = mFrameTimer.restart() * 0.001;

    glClear(GL_COLOR_BUFFER_BIT);

    mPrivateViewMatrix.rotate(10.0f * deltaTime, QVector3D{0.5f, 1.f, 0.f});
    const auto& viewMatrix = mUseGlobalMatrix ? mMainWindow->mGlobalViewMatrix : mPrivateViewMatrix;
    const auto MVP = (mPerspectiveMatrix * viewMatrix).inverted();

    auto& shader = shaderProgram("screen");
    shader.bind();
    shader.setUniformValue("MVP", MVP);

    // Volume guard automatically binds and unbinds. :)
    const auto volumeGuard = mMainWindow->mGlobalVolume->guard(0);

    mScreenVAO->draw();

    ++mFrameCount;
}