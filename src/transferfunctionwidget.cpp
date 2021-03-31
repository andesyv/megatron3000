#include "transferfunctionwidget.h"

#include <QSplineSeries>
#include <QChart>
#include <QDebug>
#include "materialnode.h"

using namespace QtCharts;

TransferFunctionWidget::TransferFunctionWidget(QWidget *parent)
    : QChartView{parent} {
    mSpline = new QSplineSeries{};
    mSpline->append(0, 0);
    mSpline->append(2, 6);
    mSpline->append(4, 2);

    mChart = new QChart{};
    mChart->legend()->hide();
    mChart->addSeries(mSpline);
    mChart->setTitle("Hi");
    mChart->createDefaultAxes();
    mChart->axes(Qt::Vertical).first()->setRange(0, 10);

    // Add some example nodes:
    mNodeScene = new QGraphicsScene{this};
    mNodeScene->addText("Hello Scene!");
    mNodes = {
        new MaterialNode{},
        new MaterialNode{},
        new MaterialNode{},
        new MaterialNode{},
    };
    std::vector<QPoint> nodepos = {{-30, -30}, {0, -23}, {16, 10}, {40, 20}};
    for (auto i{0}; i < mNodes.size(); ++i) {
        mNodes.at(i)->setPos(nodepos[i]);
        mNodeScene->addItem(mNodes.at(i));
    }


    // setChart(mChart);
    setScene(mNodeScene);
}

TransferFunctionWidget::~TransferFunctionWidget() = default;