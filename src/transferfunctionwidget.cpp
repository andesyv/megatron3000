#include "transferfunctionwidget.h"
#include <vector>
#include <utility>
#include <QVector2D>
#include "shaders/shadermanager.h"

TransferFunctionWidget::TransferFunctionWidget(QWidget* parent)
    : QOpenGLWidget{parent},
    mNodePos{{-0.2f, 0.2f}, {0.5f, 0.7f}, {-0.9f, 0.6f}, {0.2f, -0.5f}} {
    connect(this, &QOpenGLWidget::frameSwapped, this, &TransferFunctionWidget::scheduleRender);
}

void TransferFunctionWidget::initializeGL() {
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mNodeVAO);
    glBindVertexArray(mNodeVAO);

    glGenBuffers(1, &mNodeVBO);
    resizeNodeBuffer();

    glBindVertexArray(0);

    auto& SM = ShaderManager::get();

    if (!SM.valid("node")) {
        const auto shaderPath = [](auto path){
            const auto shaderpath = std::filesystem::absolute(std::filesystem::path{SHADERPATH});
            return QString::fromStdString((shaderpath / path).string());
        };

        auto& shader = SM.shader("node");

        if (!shader.addSource(QOpenGLShader::Vertex, shaderPath("node.vs"))) {
            throw std::runtime_error{"Failed to compile vertex shader"};
        }

        if (!shader.addSource(QOpenGLShader::Geometry, shaderPath("node.gs"))) {
            throw std::runtime_error{"Failed to compile vertex shader"};
        }

        if (!shader.addSource(QOpenGLShader::Fragment, shaderPath("default.fs"))) {
            throw std::runtime_error{"Failed to compile fragment shader"};
        }

        if (!shader.link()) {
            throw std::runtime_error{"Failed to link shaderprogram"};
        }
    }


    // glPointSize(10.f);
}

TransferFunctionWidget::~TransferFunctionWidget() {
    glDeleteBuffers(1, &mNodeVBO);
    glDeleteVertexArrays(1, &mNodeVAO);
}

const auto& TransferFunctionWidget::getNodesPos() const {
    return mNodePos;
}

void TransferFunctionWidget::setNodesPos(const std::vector<QVector2D>& pos) {
    mNodePos = pos;
    resizeNodeBuffer();
}

void TransferFunctionWidget::paintGL() {
    glClearColor(0.3f, 0.1f, 0.6f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    auto& shader = ShaderManager::get().shader("node");
    shader.bind();

    glBindVertexArray(mNodeVAO);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(mNodePos.size()));
    glBindVertexArray(0);
}

void TransferFunctionWidget::resizeGL(int w, int h) {

}

void TransferFunctionWidget::resizeNodeBuffer() {
    glBindVertexArray(mNodeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mNodeVBO);
    glBufferData(GL_ARRAY_BUFFER, mNodePos.size() * sizeof(QVector2D), mNodePos.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QVector2D), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}