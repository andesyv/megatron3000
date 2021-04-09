#ifndef TRANSFERFUNCTIONRENDERER_H
#define TRANSFERFUNCTIONRENDERER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QMatrix4x4>
#include <QVector2D>
#include <QColor>
#include <optional>

class NodeGlyphs;
class Spline;
class Volume;
class QVector4D;
class MainWindow;

/**
 * @brief Implementation of javascript / pythons map
 * Converts a list into another list via a transformation function.
 * @param list Input list
 * @param func Function object that will be applied on each element of list
 * @return std::vector<U> where U is return type of func
 * 
 * Example usage:
 * std::vector<int> intList = {1, 2, 3, 1, 2};
 * floatList = mapList(intList, [](auto v){ return static_cast<float>(v); });
 */
template <typename T, typename F>
auto mapList(const std::vector<T>& list, F&& func) {
    using FRetType = decltype(func(std::vector<T>::value_type{})); // This ugly line only determines the return type of func
    std::vector<FRetType> newList{};
    newList.reserve(list.size());
    std::transform(list.begin(), list.end(), std::back_inserter(newList), func);
    return newList;
}

struct Node {
    QVector2D pos;
    QColor color;

    bool operator==(const Node& rhs) const {
        return pos == rhs.pos && color == rhs.color;
    }
};

class TransferFunctionRenderer : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

public:
    TransferFunctionRenderer(QWidget *parent = nullptr);
    ~TransferFunctionRenderer();

    std::shared_ptr<Volume> getVolume() const;
    std::shared_ptr<Volume> getVolume();

    unsigned int getNodeIndex(const Node& node) const;
    unsigned int getSortedNodeIndex(const Node& node) const;

    QVector4D eval(float t) const;
    auto operator()(float t) const { return eval(t); }

    std::shared_ptr<Volume> mVolume;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    std::optional<unsigned int> isNodeIntersecting(const QVector2D& point) const;
    QVector2D screenToNormalizedCoordinates(const QPoint& point) const;

    void updateVolume();

    QMatrix4x4 mPerspMat;
    std::vector<Node> mNodes;
    std::unique_ptr<NodeGlyphs> mNodeGlyphs;
    std::unique_ptr<Spline> mSpline;

    const float mNodeRadius = 0.07f;
    std::optional<unsigned int> mDraggedNode;
    QVector2D mLastMousePos;

    MainWindow* mMainWindow{nullptr};

    int mSelectedNode{-1};


public slots:
    void nodesChanged();

signals:
    void nodeSelected(Node& node);

};

#endif // TRANSFERFUNCTIONRENDERER_H
