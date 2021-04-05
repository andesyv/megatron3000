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
    mVolume = mMainWindow->mGlobalVolume;

    //DEBUGGING


    // Histogram chart:
    QLineSeries *series = new QLineSeries();
    for (int i = 0; i < 256; i++) {
        series->append(i,i);
    }

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();

    QFont font;
    font.setPixelSize(18);
    chart->setTitleFont(font);
    chart->setTitleBrush(QBrush(Qt::black));
    chart->setTitle("Histogram");

    QPen pen(QRgb(0x000000));
    pen.setWidth(5);
    series->setPen(pen);

    chart->setAnimationOptions(QChart::AllAnimations);

    QCategoryAxis *axisX = new QCategoryAxis();
    for (int i = 0; i < 256; i++) {
        axisX->append(QString::number(i),i);
    }

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);


    mLayout->addWidget(chartView);
}

//HistogramWidget::createDataHistogram()

HistogramWidget::~HistogramWidget() = default;
