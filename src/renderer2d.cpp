#include "renderer2d.h"
#include "mainwindow.h"
#include "volume.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

Renderer2D::Renderer2D(QWidget *parent)
    : Renderer{parent} {
}

void Renderer2D::initializeGL() {
    Renderer::initializeGL();

    mPrivateViewMatrix.setToIdentity();
}

void Renderer2D::paintGL() {
    const float deltaTime = mFrameTimer.restart() * 0.001;

    glClear(GL_COLOR_BUFFER_BIT);

    // mScreenVAO->draw();

    ++mFrameCount;
}