#include "transferfunctionwidget.h"
#include <vector>
#include "renderutils.h"
#include "spline.h"
#include <QMouseEvent>
#include "volume.h"
#include "mainwindow.h"
#include <QVector4D>
#include <algorithm>
#include <utility>

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
    glClearColor(0.7f, 0.7f, 0.7f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    const auto aspectRatio = width() / static_cast<float>(height());

    mSpline->draw(getVolume());
    
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
        // Clamp:
        const auto aspectRatio = width() / static_cast<float>(height());
        const auto scrScale = aspectScale(aspectRatio);
        const auto nodeRadius = scrScale * mNodeRadius;

        mNodePos[*mDraggedNode].setX(std::clamp(mNodePos[*mDraggedNode].x(), -1.f + nodeRadius.x(), 1.f - nodeRadius.x()));
        mNodePos[*mDraggedNode].setY(std::clamp(mNodePos[*mDraggedNode].y(), -1.f + nodeRadius.y(), 1.f - nodeRadius.y()));

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

float linearBetween(float a, float b, float p) {
    if (p < a)
        return 0.0;
    else if (b < p)
        return 1.0;
    
    return (p - a) / (b - a);
}

// Assumes points are sorted
float findTHorizontally(const std::vector<QVector2D>& p, const QVector2D& point) {
    if (p.size() < 2)
        return 0.0;

    return linearBetween(-1.f, 1.f, point.x());
}


void TransferFunctionWidget::updateVolume() {
    auto& volume = getVolume();
    if (!volume || mNodePos.size() < 2)
        return;

    /** B(t) describes a spline that is continous for t = [0, 1],
     * but the segments ( B(t) dt ) have no guarantee being
     * similar / consistently spaced. Thus finding the t that
     * gives a point B on a spline is a difficult task. See
     * https://math.stackexchange.com/questions/527005/find-value-of-t-at-a-point-on-a-cubic-bezier-curve
     * for an actual solution. I'm not that smart so instead
     * I just calculate 10 times the integrals and take the
     * average pos.
     * TODO: Calculate this shit and use that instead
     */
    const auto resolution = 64;
    const auto evalResolution = 10 * resolution;
    const auto dres = static_cast<double>(evalResolution - 1);
    auto sortedPoints{mNodePos};
    std::sort(sortedPoints.begin(), sortedPoints.end(), [](const auto& a, const auto& b){ return a.x() < b.x(); });
    
    std::vector<std::pair<QVector4D, unsigned int>> valueBuckets;
    valueBuckets.resize(resolution, {QVector4D{}, 0});

    for (int i {0}; i < evalResolution; ++i) {
        const double t = i / dres;
        const auto val = mSpline->eval(t);
        const auto x = findTHorizontally(sortedPoints, val);
        const auto bucketI = static_cast<unsigned int>(x * (resolution - 1));
        auto& [bval, bcount] = valueBuckets.at(bucketI);
        bval += QVector4D{0.f, 0.f, 0.f, val.y()};
        ++bcount;
    }

    // Average values:
    std::vector<QVector4D> values;
    values.reserve(resolution);

    bool bUpper = false;
    for (const auto& [sum, num] : valueBuckets) {
        if (num != 0) {
            // Fill middle values with buckets
            values.push_back(sum / num);
            bUpper = true;
        } else {
            // Fill upper values with last node:
            if (bUpper)
                values.push_back(QVector4D{0.f, 0.f, 0.f, sortedPoints.back().y()});
            else
                values.push_back(QVector4D{0.f, 0.f, 0.f, sortedPoints.front().y()});
        }
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