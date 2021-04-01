#include "transferfunctionwidget.h"
#include <vector>
#include <utility>
#include <QVector2D>
#include "shaders/shadermanager.h"
#include "renderutils.h"
#include "spline.h"
#include <QMouseEvent>

TransferFunctionWidget::TransferFunctionWidget(QWidget* parent)
    : QOpenGLWidget{parent},
    mNodePos{{-0.2f, 0.2f}, {0.5f, 0.7f}, {-0.9f, 0.6f}, {0.2f, -0.5f}, {0.8f, 0.6f}} {
    connect(this, &QOpenGLWidget::frameSwapped, this, &TransferFunctionWidget::scheduleRender);
}

void TransferFunctionWidget::initializeGL() {
    initializeOpenGLFunctions();

    mNodeGlyphs = std::make_unique<NodeGlyphs>(mNodePos);
    mSpline = std::make_unique<Spline>(mNodePos, 30);

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

    mSpline->draw();
    
    mNodeGlyphs->draw(mNodeRadius);
}

// void TransferFunctionWidget::resizeGL(int w, int h) {

// }

void TransferFunctionWidget::mouseMoveEvent(QMouseEvent *event) {
    if (mDraggedNode) {
        const auto mousePos = screenToNormalizedCoordinates(event->pos());
        const auto delta = mousePos - mLastMousePos;
        mLastMousePos = mousePos;
        
        mNodePos[*mDraggedNode] += delta;

        nodesChanged();
    }
}

void TransferFunctionWidget::mousePressEvent(QMouseEvent *event) {
    const auto mousePos = screenToNormalizedCoordinates(event->pos());
    const auto index = isNodeIntersecting(mousePos);
    if (index) {
        mLastMousePos = mousePos;
        mDraggedNode = index;
    }
}

void TransferFunctionWidget::mouseReleaseEvent(QMouseEvent *event) {
    mDraggedNode = std::nullopt;
}

std::optional<unsigned int> TransferFunctionWidget::isNodeIntersecting(const QVector2D& point) const {
    for (unsigned int i{0}; i < mNodePos.size(); ++i)
        if ((point - mNodePos[i]).length() - mNodeRadius < 0.0)
            return i;
    
    return std::nullopt;
}

QVector2D TransferFunctionWidget::screenToNormalizedCoordinates(const QPoint& point) const {
    const auto x = point.x() / static_cast<double>(width());
    const auto y = point.y() / static_cast<double>(height());
    return {
        static_cast<float>(x * 2.0 - 1.0),
        -static_cast<float>(y * 2.0 - 1.0)
    };
}

void TransferFunctionWidget::scheduleRender() {
    // Only render whenever we need to update the screen, at most once per screen refresh rate:
    if (mNeedsUpdate) {
        mNeedsUpdate = false;
        update();
    }
}

void TransferFunctionWidget::nodesChanged() {
    mNodeGlyphs->updateNodeBuffer(mNodePos);
    mSpline->update(mNodePos);

    mNeedsUpdate = true;
}