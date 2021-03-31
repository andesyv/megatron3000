#include "transferfunctionwidget.h"
#include <vector>
#include <utility>
#include <QVector2D>
#include "shaders/shadermanager.h"
#include "renderutils.h"

TransferFunctionWidget::TransferFunctionWidget(QWidget* parent)
    : QOpenGLWidget{parent},
    mNodePos{{-0.2f, 0.2f}, {0.5f, 0.7f}, {-0.9f, 0.6f}, {0.2f, -0.5f}} {
    connect(this, &QOpenGLWidget::frameSwapped, this, &TransferFunctionWidget::scheduleRender);
}

void TransferFunctionWidget::initializeGL() {
    initializeOpenGLFunctions();

    mNodeGlyphs = std::make_unique<NodeGlyphs>(mNodePos);

    // glPointSize(10.f);
}

TransferFunctionWidget::~TransferFunctionWidget() {
    
}

const auto& TransferFunctionWidget::getNodesPos() const {
    return mNodePos;
}

void TransferFunctionWidget::setNodesPos(const std::vector<QVector2D>& pos) {
    mNodePos = pos;
    mNodeGlyphs->resizeNodeBuffer(pos);
}

void TransferFunctionWidget::paintGL() {
    glClearColor(0.3f, 0.1f, 0.6f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    mNodeGlyphs->draw();
}

void TransferFunctionWidget::resizeGL(int w, int h) {

}