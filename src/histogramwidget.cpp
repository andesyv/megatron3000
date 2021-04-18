#include "histogramwidget.h"
#include "ui_histogramwidget.h"
#include <QtCharts>
#include <QVBoxLayout>
#include "volume.h"
#include "mainwindow.h"

HistogramWidget::HistogramWidget(QWidget *parent) :
    QWidget(parent)
{
    // Layout:
    mLayout = new QVBoxLayout{this};
    mLayout->setContentsMargins(0, 0, 0, 0);

    /** Get volume from main window:
     *  This only loads data if it is already present in main window.
     */
    auto widget = window();
    while (widget != nullptr && !widget->isWindow())
        widget = widget->window();
    mMainWindow = dynamic_cast<MainWindow*>(widget);

    drawHistogram();
}

void HistogramWidget::drawHistogram()
{
    // TODO: Import actual data
    unsigned long histogramData[256];
    QList<QBarSet*> bins;
    QBarSeries *series = new QBarSeries();
    QChart *chart = new QChart();
    QChartView *chartView;
    for (int i = 0; i < 256; i++){
        // Random data
        histogramData[i] = (int) qrand() % 1000;
        QBarSet *bar = new QBarSet(QString::number((i+1)));
        *bar << histogramData[i];
        bar->setColor(Qt::black);
        bins.append(bar);
        series->append(bar);
    }

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0,1000);
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->legend()->hide();
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    mLayout->addWidget(chartView);
}

HistogramWidget::~HistogramWidget() = default;

std::shared_ptr<Volume> HistogramWidget::getVolume() {
    return mMainWindow->mGlobalVolume;
}
