#ifndef TRANSFERFUNCTIONWIDGET_H
#define TRANSFERFUNCTIONWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QMatrix4x4>

class NodeGlyphs;

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

    QMatrix4x4 mPerspMat;
    std::vector<QVector2D> mNodePos;
    std::unique_ptr<NodeGlyphs> mNodeGlyphs;


private slots:
    void scheduleRender() {
        paintGL();
    }

};

#endif // TRANSFERFUNCTIONWIDGET_H
