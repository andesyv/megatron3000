#include "transferfunctionwidget.h"

#include <QSplineSeries>
#include <QChart>
#include <QDebug>
#include <QGraphicsScene>
#include "materialnode.h"

using namespace QtCharts;

// Note: Conversion between spline and points can be used with:
// QPointF QChart::mapToPosition(const QPointF &value, QAbstractSeries *series)
// QPointF QChart::mapToValue(const QPointF &position, QAbstractSeries *series)

TransferFunctionWidget::TransferFunctionWidget(QWidget *parent)
    : QGraphicsView{} {

    // setResizeAnchor(ViewportAnchor::AnchorViewCenter);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto scene = new QGraphicsScene{this};
    scene->addText("Hello Scene!");
    setScene(scene);

    // Create spline
    mSpline = new QSplineSeries{};
    mSpline->append(0, 0);
    mSpline->append(2, 6);
    mSpline->append(4, 2);

    // Create chart to keep hold spline
    mChart = new QChart{};
    mChart->setMinimumSize(100, 100);
    mChart->legend()->hide();
    mChart->addSeries(mSpline);
    // mChart->setTitle("Hi");
    // mChart->createDefaultAxes();
    // mChart->axes(Qt::Vertical).first()->setRange(0, 10);
    scene->addItem(mChart);

    // Add some example nodes:
    for (auto coords : std::vector<QPoint>{{100, 130}, {23, 89}, {16, 10}, {40, 20}}) {
        auto node = new MaterialNode{};
        mNodes.push_back(node);
        node->setPos(coords);
        // mNodeScene->addItem(node);
        scene->addItem(node);
    }


    // mNodeScene->addWidget(mChart);
    // setScene(mNodeScene);

    // Apparantly have to manually free previous chart before setting new (Qt pls...)
    // if (auto c = chart())
    //     delete c;
    // setChart(mChart);
}

TransferFunctionWidget::~TransferFunctionWidget() = default;

void TransferFunctionWidget::resizeEvent(QResizeEvent *event) {
    mChart->resize(event->size());
    // // qDebug() << "Resize!";
    fitInView(mChart, Qt::KeepAspectRatio);
}