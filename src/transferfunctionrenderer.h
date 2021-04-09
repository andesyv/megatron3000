#ifndef TRANSFERFUNCTIONRENDERER_H
#define TRANSFERFUNCTIONRENDERER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QMatrix4x4>
#include <QVector2D>
#include <optional>

class NodeGlyphs;
class Spline;
class Volume;
class QVector4D;
class MainWindow;

class TransferFunctionRenderer : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

public:
    TransferFunctionRenderer(QWidget *parent = nullptr);
    ~TransferFunctionRenderer();

    std::shared_ptr<Volume> getVolume() const;
    std::shared_ptr<Volume> getVolume();

    QVector4D eval(float t) const;
    auto operator()(float t) const { return eval(t); }

    std::shared_ptr<Volume> mVolume;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    std::optional<unsigned int> isNodeIntersecting(const QVector2D& point) const;
    QVector2D screenToNormalizedCoordinates(const QPoint& point) const;

    void updateVolume();

    QMatrix4x4 mPerspMat;
    std::vector<QVector2D> mNodePos;
    std::unique_ptr<NodeGlyphs> mNodeGlyphs;
    std::unique_ptr<Spline> mSpline;

    const float mNodeRadius = 0.04f;
    std::optional<unsigned int> mDraggedNode;
    QVector2D mLastMousePos;

    MainWindow* mMainWindow{nullptr};


private slots:
    void nodesChanged();

};

#endif // TRANSFERFUNCTIONRENDERER_H
