#ifndef TRANSFERFUNCTIONWIDGET_H
#define TRANSFERFUNCTIONWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <vector>

class QGraphicsScene;
class MaterialNode;

namespace QtCharts {
class QSplineSeries;
class QChart;
}

class TransferFunctionWidget : public QGraphicsView
{
    Q_OBJECT

public:
    explicit TransferFunctionWidget(QWidget *parent = nullptr);
    ~TransferFunctionWidget();

    std::vector<MaterialNode*> mNodes;

private:
    void resizeEvent(QResizeEvent *event) override;

    QtCharts::QSplineSeries* mSpline{nullptr};
    QtCharts::QChart* mChart{nullptr};

    QGraphicsScene* mNodeScene{nullptr};
};

#endif // TRANSFERFUNCTIONWIDGET_H
