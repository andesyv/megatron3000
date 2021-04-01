#ifndef TRANSFERFUNCTIONWIDGET_H
#define TRANSFERFUNCTIONWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QMatrix4x4>
#include <QVector2D>
#include <optional>

class NodeGlyphs;
class Spline;

class TransferFunctionWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

public:
    TransferFunctionWidget(QWidget *parent = nullptr);
    ~TransferFunctionWidget();

    const auto& getNodesPos() const;
    void setNodesPos(const std::vector<QVector2D>& pos);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    std::optional<unsigned int> isNodeIntersecting(const QVector2D& point) const;
    QVector2D screenToNormalizedCoordinates(const QPoint& point) const;

    QMatrix4x4 mPerspMat;
    std::vector<QVector2D> mNodePos;
    std::unique_ptr<NodeGlyphs> mNodeGlyphs;
    std::unique_ptr<Spline> mSpline;

    const float mNodeRadius = 0.02f;
    std::optional<unsigned int> mDraggedNode;
    QVector2D mLastMousePos;


private slots:
    void nodesChanged();

};

#endif // TRANSFERFUNCTIONWIDGET_H
