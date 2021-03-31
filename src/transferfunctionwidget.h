#ifndef TRANSFERFUNCTIONWIDGET_H
#define TRANSFERFUNCTIONWIDGET_H

#include <QWidget>
#include <QChartView>
#include <vector>

class QGraphicsScene;
class MaterialNode;

namespace QtCharts {
class QSplineSeries;
class QChart;
}

class TransferFunctionWidget : public QtCharts::QChartView
{
    Q_OBJECT

public:
    explicit TransferFunctionWidget(QWidget *parent = nullptr);
    ~TransferFunctionWidget();

    std::vector<MaterialNode*> mNodes;

private:
    QtCharts::QSplineSeries* mSpline{nullptr};
    QtCharts::QChart* mChart{nullptr};

    QGraphicsScene* mNodeScene{nullptr};
};

#endif // TRANSFERFUNCTIONWIDGET_H
