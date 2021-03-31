#include "transferfunctionwidget.h"
#include <vector>
#include <utility>
#include <QVector2D>
#include "shaders/shadermanager.h"

TransferFunctionWidget::TransferFunctionWidget(QWidget* parent)
    : QOpenGLWidget{parent} {
    connect(this, &QOpenGLWidget::frameSwapped, this, &TransferFunctionWidget::scheduleRender);
}

void TransferFunctionWidget::initializeGL() {
    initializeOpenGLFunctions();

    struct vec2 {
        float x, y;
    };

    std::vector<vec2> coords{{-0.2, 0.2}, {0.5, 0.7}, {-0.9, 0.6}, {0.2, -0.5}};
    mNodes.reserve(coords.size());
    for (auto pos : coords) {
        Node n;
        glGenVertexArrays(1, &n.vao);
        glBindVertexArray(n.vao);

        glGenBuffers(1, &n.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, n.vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pos), &pos.x, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), nullptr);
        glEnableVertexAttribArray(0);

        mNodes.push_back(n);
    }
    glBindVertexArray(0);

    auto& SM = ShaderManager::get();

    if (!SM.valid("node")) {
        const auto shaderpath = std::filesystem::absolute(std::filesystem::path{SHADERPATH});
        const auto vspath = QString::fromStdString((shaderpath / "node.vs").string());
        const auto fspath = QString::fromStdString((shaderpath / "default.fs").string());

        auto& shader = SM.shader("node");

        if (!shader.addSource(QOpenGLShader::Vertex, vspath)) {
            throw std::runtime_error{"Failed to compile vertex shader"};
        }

        if (!shader.addSource(QOpenGLShader::Fragment, fspath)) {
            throw std::runtime_error{"Failed to compile fragment shader"};
        }

        if (!shader.link()) {
            throw std::runtime_error{"Failed to link shaderprogram"};
        }
    }


    glPointSize(10.f);
}

void TransferFunctionWidget::paintGL() {
    glClearColor(0.3f, 0.1f, 0.6f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    auto& shader = ShaderManager::get().shader("node");
    shader.bind();

    for (const auto& node : mNodes) {
        glBindVertexArray(node.vao);
        glDrawArrays(GL_POINTS, 0, 1);
    }
}

void TransferFunctionWidget::resizeGL(int w, int h) {

}