#ifndef TRANSFERFUNCTIONWIDGET_H
#define TRANSFERFUNCTIONWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QMatrix4x4>

struct Node {
    GLuint vao, vbo;
};

class TransferFunctionWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

public:
    TransferFunctionWidget(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    QMatrix4x4 mPerspMat;
    std::vector<Node> mNodes;


private slots:
    void scheduleRender() {
        paintGL();
    }
};

#endif // TRANSFERFUNCTIONWIDGET_H
