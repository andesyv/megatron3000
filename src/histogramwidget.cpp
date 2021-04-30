#include "histogramwidget.h"
#include <QtCharts>
#include <QVBoxLayout>
#include "volume.h"
#include "mainwindow.h"
#include <QChartView>

#include <QtConcurrentRun>

using namespace QtCharts;

HistogramWidget::HistogramWidget(QWidget *parent) :
    QWidget(parent),
    IMenu{this}
{
    // Layout:
    mLayout = new QVBoxLayout{this};
    mLayout->setContentsMargins(0, 0, 0, 0);

    mLayout->addWidget(mMenuBar);

    createView();

    // Connect future watcher (concurrency)
    connect(&mWatcher, &QFutureWatcher<QChart*>::finished, this, &HistogramWidget::finishHistogramGeneration);

    // Render histogram if we have data for it:
    if (mVolume)
        drawHistogram();
}

void HistogramWidget::drawHistogram()
{
    // Initialize async func:
    mFuture = QtConcurrent::run(&HistogramWidget::generateHistogram, mVolume);
    mWatcher.setFuture(mFuture);
}

HistogramWidget::~HistogramWidget() = default;

std::shared_ptr<Volume> HistogramWidget::getVolume() {
    return mVolume;
}

void HistogramWidget::volumeSwitched() {
    // Redraw histogram if data changed
    drawHistogram();
}

void HistogramWidget::createView() {
    // Make dummy chart:
    mChart = new QChart{};
    mChart->createDefaultAxes();
    mChart->legend()->hide();

    // Create chartview:
    mChartView = new QChartView{mChart, this};
    mChartView->setRenderHint(QPainter::Antialiasing);
    mLayout->addWidget(mChartView);
}

QVector<qreal> HistogramWidget::generateHistogram(std::shared_ptr<Volume> volume) {
    // If volume is empty, exit early.
    if (!volume)
        return {};
    
    const unsigned int BIN_COUNT = 256;
    const unsigned int VALUE_COUNT = static_cast<unsigned int>(volume->data().size());

    // Note: Using QVector because QList is apparantly a linked list and doesn't have random access.
    // Not sure why Qt decided it was a good idea to therefore implement everything using QLists...
    QVector<qreal> bins;
    bins.resize(BIN_COUNT);

    // Inject data into bins:
    for (auto value : volume->data()) {
        // Data should already be normalized so index can be found with floor(value * BIN_COUNT)
        // (clamp just in case)
        const int binIndex = std::clamp(static_cast<int>(value * BIN_COUNT), 0, static_cast<int>(BIN_COUNT) - 1);
        // Increment bins voxel count
        // Sum of all bins should be 1, so divide by value count.
        bins[binIndex] += 1.0 / VALUE_COUNT;
    }

    return bins;
}

void HistogramWidget::finishHistogramGeneration() {
    // At this point the future "should" have it's data,
    // so this avoids blocking the thread
    auto bins = mWatcher.result();
    auto chart = new QChart{};

    auto series = new QBarSeries();
    auto bars = new QBarSet{"Densities"};
    bars->setColor(Qt::black);
    bars->append(QList<qreal>::fromVector(bins));
    series->append(bars);

    auto axisY = new QValueAxis;
    axisY->setRange(0,1000);
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->legend()->hide();
    // Set new chart
    mChartView->setChart(chart);
    // delete old chart if it exists
    if (mChart)
        delete mChart;
    mChart = chart;
}