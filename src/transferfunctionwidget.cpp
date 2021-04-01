#include "transferfunctionwidget.h"
#include <vector>
#include "renderutils.h"
#include "spline.h"
#include <QMouseEvent>
#include "volume.h"
#include "mainwindow.h"
#include <QVector4D>

TransferFunctionWidget::TransferFunctionWidget(QWidget* parent)
    : QOpenGLWidget{parent},
    mNodePos{{-0.2f, 0.2f}, {0.5f, 0.7f}, {-0.9f, 0.6f}, {0.2f, -0.5f}, {0.8f, 0.6f}} {

    // Follow outer chain to find main window:
    auto widget = window();
    while (widget != nullptr && !widget->isWindow())
        widget = widget->window();
    
    mMainWindow = dynamic_cast<MainWindow*>(widget);


    /// NOTE: Can't do this because this will create race conditions with other transfer function widgets
    // Connect event to eventually load volume:
    // connect(mVolume.get(), &Volume::loaded, this, &TransferFunctionWidget::updateVolume);
}

void TransferFunctionWidget::initializeGL() {
    initializeOpenGLFunctions();

    mNodeGlyphs = std::make_unique<NodeGlyphs>(mNodePos);
    mSpline = std::make_unique<Spline>(mNodePos, 30);
}

TransferFunctionWidget::~TransferFunctionWidget() = default;

std::shared_ptr<Volume> TransferFunctionWidget::getVolume() const {
    return mMainWindow->mGlobalVolume;
}

std::shared_ptr<Volume> TransferFunctionWidget::getVolume() {
    return mMainWindow->mGlobalVolume;
}

QVector4D TransferFunctionWidget::eval(float t) const {
    return QVector4D{0.f, 0.f, 0.f, mSpline->eval(t).y()};
}

void TransferFunctionWidget::paintGL() {
    glClearColor(0.3f, 0.1f, 0.6f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    const auto aspectRatio = width() / static_cast<float>(height());

    mSpline->draw();
    
    mNodeGlyphs->draw(aspectRatio, mNodeRadius);
}

void TransferFunctionWidget::resizeGL(int w, int h) {
    update();
}

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
        // If right clicking on a node we want it to remove it instead of moving it
        if (event->button() == Qt::MouseButton::RightButton) {
            
            // Don't remove nodes when there's 2 or less.
            if (mNodePos.size() < 3)
                return;

            mNodePos.erase(mNodePos.begin() + *index);
            nodesChanged();
        
        // If normal clicking we want it to start moving the node
        } else {
            mLastMousePos = mousePos;
            mDraggedNode = index;
        }
    
    // Clicking outside a node should just add a node
    } else {
        mNodePos.push_back(mousePos);
        nodesChanged();
    }
}

void TransferFunctionWidget::mouseReleaseEvent(QMouseEvent *event) {
    mDraggedNode = std::nullopt;
}

std::optional<unsigned int> TransferFunctionWidget::isNodeIntersecting(const QVector2D& point) const {
    const auto aspectRatio = width() / static_cast<float>(height());
    const auto scrScale = aspectScale(aspectRatio);
    for (unsigned int i{0}; i < mNodePos.size(); ++i) {
        const auto dir = (point - mNodePos[i]);
        if ((dir * scrScale).length() - mNodeRadius < 0.0)
            return i;
    }
    
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

void TransferFunctionWidget::updateVolume() {
    auto& volume = getVolume();
    if (!volume)
        return;

    const auto resolution = 64;
    const auto dres = static_cast<double>(resolution - 1);
    
    std::vector<QVector4D> values;
    values.reserve(resolution);

    for (int i {0}; i < resolution; ++i) {
        const double t = i / dres;
        values.push_back(eval(t));
    }

    volume->updateTransferFunction(values);
}

void TransferFunctionWidget::nodesChanged() {
    mNodeGlyphs->updateNodeBuffer(mNodePos);
    mSpline->update(mNodePos);

    // Update volumes transfer function:
    updateVolume();

    update();
}