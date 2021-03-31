#ifndef TRANSFERFUNCTIONWIDGET_H
#define TRANSFERFUNCTIONWIDGET_H

#include <QWidget>
#include <QChartView>

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

private:
    QtCharts::QSplineSeries* mSpline{nullptr};
    QtCharts::QChart* mChart{nullptr};
};

#endif // TRANSFERFUNCTIONWIDGET_H
