#include "histogramwidget.h"
#include "ui_histogramwidget.h"
#include <QtCharts>
#include <QVBoxLayout>
#include "volume.h"
#include "mainwindow.h"
#include <QChartView>

using namespace QtCharts;

HistogramWidget::HistogramWidget(QWidget *parent) :
    QWidget(parent),
    IMenu{this}
{
    // Layout:
    mLayout = new QVBoxLayout{this};
    mLayout->setContentsMargins(0, 0, 0, 0);

    mLayout->addWidget(mMenuBar);

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
    
    auto chartView = new QChartView{chart, this};
    chartView->setRenderHint(QPainter::Antialiasing);

    // If there was already a previous chartview, replace it. Otherwise just add it.
    if (mChartView) {
        const auto old = mLayout->replaceWidget(mChartView, chartView);
        delete old;
    } else
        mLayout->addWidget(chartView);

    // Set new chartview:
    mChartView = chartView;
}

HistogramWidget::~HistogramWidget() = default;

std::shared_ptr<Volume> HistogramWidget::getVolume() {
    return mVolume;
}

void HistogramWidget::volumeSwitched() {

}