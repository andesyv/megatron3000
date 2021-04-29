#include "transferfunctionrenderer.h"
#include <vector>
#include "renderutils.h"
#include "spline.h"
#include <QMouseEvent>
#include "volume.h"
#include "mainwindow.h"
#include <QVector4D>
#include <algorithm>
#include <utility>

TransferFunctionRenderer::TransferFunctionRenderer(QWidget* parent)
    : QOpenGLWidget{parent},
    mNodes{{{-0.8f, 0.f}}, {{0.8f, 0.8f}}} {

    // Follow outer chain to find main window:
    auto widget = window();
    while (widget != nullptr && !widget->isWindow())
        widget = widget->window();
    
    mMainWindow = dynamic_cast<MainWindow*>(widget);
}

void TransferFunctionRenderer::initializeGL() {
    initializeOpenGLFunctions();

    const auto positions = mapList(mNodes, [](const auto& n){ return n.pos; });
    mNodeGlyphs = std::make_unique<NodeGlyphs>(positions);
    mSpline = std::make_unique<Spline>(positions, 30);
}

TransferFunctionRenderer::~TransferFunctionRenderer() = default;

unsigned int TransferFunctionRenderer::getNodeIndex(const Node& node) const {
    return std::find(mNodes.begin(), mNodes.end(), node) - mNodes.begin();
}

unsigned int TransferFunctionRenderer::getSortedNodeIndex(const Node& node) const {
    auto pos{mNodes};
    std::sort(pos.begin(), pos.end(), [](const auto& a, const auto& b){ return a.pos.x() < b.pos.x(); });
    return std::find(pos.begin(), pos.end(), node) - pos.begin();
}

QVector4D TransferFunctionRenderer::eval(float t) const {
    return QVector4D{0.f, 0.f, 0.f, mSpline->eval(t).y()};
}

void TransferFunctionRenderer::paintGL() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const auto aspectRatio = width() / static_cast<float>(height());

    mSpline->draw(mVolume);
    mNodeGlyphs->draw(aspectRatio, mNodeRadius, mSelectedNode);

    glDisable(GL_BLEND);
}

void TransferFunctionRenderer::resizeGL(int w, int h) {
    update();
}

void TransferFunctionRenderer::mouseMoveEvent(QMouseEvent *event) {
    if (mDraggedNode) {
        const auto mousePos = screenToNormalizedCoordinates(event->pos());
        const auto delta = mousePos - mLastMousePos;
        mLastMousePos = mousePos;
        
        auto& nodePos = mNodes[*mDraggedNode].pos;
        nodePos += delta;
        // Clamp:
        const auto aspectRatio = width() / static_cast<float>(height());
        const auto scrScale = aspectScale(aspectRatio);
        const auto nodeRadius = scrScale * mNodeRadius;

        nodePos.setX(std::clamp(nodePos.x(), -1.f + nodeRadius.x(), 1.f - nodeRadius.x()));
        nodePos.setY(std::clamp(nodePos.y(), -1.f + nodeRadius.y(), 1.f - nodeRadius.y()));

        nodesChanged();
    }
}

void TransferFunctionRenderer::mousePressEvent(QMouseEvent *event) {
    const auto mousePos = screenToNormalizedCoordinates(event->pos());
    const auto index = isNodeIntersecting(mousePos);

    if (index) {
        // If right clicking on a node we want it to remove it instead of moving it
        if (event->button() == Qt::MouseButton::RightButton) {
            
            // Don't remove nodes when there's 2 or less.
            if (mNodes.size() < 3)
                return;

            mNodes.erase(mNodes.begin() + *index);
            nodesChanged();
        
        // If normal clicking we want it to start moving the node
        } else {
            mLastMousePos = mousePos;
            mDraggedNode = index;

            auto& node = mNodes[*index];

            mSelectedNode = getNodeIndex(node);
            nodeSelected(node);

            nodesChanged();
        }
    }
}

void TransferFunctionRenderer::mouseDoubleClickEvent(QMouseEvent *event) {
    const auto mousePos = screenToNormalizedCoordinates(event->pos());
    const auto index = isNodeIntersecting(mousePos);

    // Double clicking outside a node should just add a node
    if (!index) {
        mNodes.push_back({mousePos});
        auto& newNode = mNodes.back();
        
        mSelectedNode = getNodeIndex(newNode);
        nodeSelected(newNode);
        
        nodesChanged();
    }
}

void TransferFunctionRenderer::mouseReleaseEvent(QMouseEvent *event) {
    mDraggedNode = std::nullopt;
}

std::optional<unsigned int> TransferFunctionRenderer::isNodeIntersecting(const QVector2D& point) const {
    const auto aspectRatio = width() / static_cast<float>(height());
    const auto scrScale = aspectScale(aspectRatio);
    for (unsigned int i{0}; i < mNodes.size(); ++i) {
        const auto dir = (point - mNodes[i].pos);
        if ((dir * scrScale).length() - mNodeRadius < 0.0)
            return i;
    }
    
    return std::nullopt;
}

QVector2D TransferFunctionRenderer::screenToNormalizedCoordinates(const QPoint& point) const {
    return screenPointToNormalizedCoordinates(point, width(), height());
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


void TransferFunctionRenderer::updateVolume() {
    if (!mVolume || mNodes.size() < 2)
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
    auto sortedPoints{mNodes};
    std::sort(sortedPoints.begin(), sortedPoints.end(), [](const auto& a, const auto& b){ return a.pos.x() < b.pos.x(); });
    const auto sortedPos = mapList(mNodes, [](const auto& n){ return n.pos; });

    const auto colors = mapList(sortedPoints, [](const auto& n){
        return QVector3D{
            static_cast<float>(n.color.redF()),
            static_cast<float>(n.color.greenF()),
            static_cast<float>(n.color.blueF())
        };
    });
    
    std::vector<std::pair<QVector4D, unsigned int>> valueBuckets;
    valueBuckets.resize(resolution, {QVector4D{}, 0});

    for (int i {0}; i < evalResolution; ++i) {
        const double t = i / dres;
        const auto val = mSpline->eval(t);
        const auto color = bezier(colors, t);
        const auto x = findTHorizontally(sortedPos, val);
        const auto bucketI = static_cast<unsigned int>(x * (resolution - 1));
        auto& [bval, bcount] = valueBuckets.at(bucketI);
        bval += QVector4D{color, val.y()};
        ++bcount;
    }

    // Average values:
    std::vector<QVector4D> values;
    values.reserve(resolution);

    bool bUpper = false;
    for (const auto& [sum, num] : valueBuckets) {
        QVector4D val{};
        if (num != 0) {
            // Fill middle values with buckets
            val = sum / num;
            bUpper = true;
        } else {
            // Fill upper values with last node:
            if (bUpper)
                val = QVector4D{colors.back(), sortedPoints.back().pos.y()};
            else
                val = QVector4D{colors.front(), sortedPoints.front().pos.y()};
        }
        // [-1, 1] -> [0, 1]
        val.setY(val.y() * 0.5f + 0.5f);
        values.push_back(val);
    }

    mVolume->updateTransferFunction(values);
}

void TransferFunctionRenderer::nodesChanged() {
    const auto positions = mapList(mNodes, [](const auto& n){ return n.pos; });

    mNodeGlyphs->updateNodeBuffer(positions);
    mSpline->update(positions);

    // Update volumes transfer function:
    updateVolume();

    update();
}
