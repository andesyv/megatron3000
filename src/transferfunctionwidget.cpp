#include "transferfunctionwidget.h"

#include <QSplineSeries>
#include <QChart>
#include <QDebug>

using namespace QtCharts;

TransferFunctionWidget::TransferFunctionWidget(QWidget *parent) :
    QChartView{parent} {
    mSpline = new QSplineSeries{};
    mSpline->append(0, 6);
    mSpline->append(2, 4);
    mSpline->append(3, 8);
    mSpline->append(7, 4);
    mSpline->append(10, 5);

    mChart = new QChart{};
    mChart->legend()->hide();
    mChart->addSeries(mSpline);
    mChart->setTitle("Hi");
    mChart->createDefaultAxes();
    mChart->axes(Qt::Vertical).first()->setRange(0, 10);

    setChart(mChart);
}

TransferFunctionWidget::~TransferFunctionWidget() = default;