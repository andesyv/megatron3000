#ifndef TRANSFERFUNCTIONWIDGET_H
#define TRANSFERFUNCTIONWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QMatrix4x4>

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

    void resizeNodeBuffer();

    QMatrix4x4 mPerspMat;
    GLuint mNodeVAO, mNodeVBO;
    std::vector<QVector2D> mNodePos;


private slots:
    void scheduleRender() {
        paintGL();
    }

};

#endif // TRANSFERFUNCTIONWIDGET_H
