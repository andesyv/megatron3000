#include "histogramwidget.h"
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

    // Render histogram if we have data for it:
    if (mVolume)
        drawHistogram();
}

void HistogramWidget::drawHistogram()
{
    // If volume is empty, exit early.
    if (!mVolume)
        return;
    
    const unsigned int BIN_COUNT = 256;
    const unsigned int VALUE_COUNT = static_cast<unsigned int>(mVolume->data().size());

    // Note: Using QVector because QList is apparantly a linked list and doesn't have random access.
    // Not sure why Qt decided it was a good idea to therefore implement everything using QLists...
    QVector<qreal> bins;
    bins.resize(BIN_COUNT);

    // Inject data into bins:
    for (auto value : mVolume->data()) {
        // Data should already be normalized so index can be found with floor(value * BIN_COUNT)
        // (clamp just in case)
        const int binIndex = std::clamp(static_cast<int>(value * BIN_COUNT), 0, static_cast<int>(BIN_COUNT) - 1);
        // Increment bins voxel count
        // Sum of all bins should be 1, so divide by value count.
        bins[binIndex] += 1.0 / VALUE_COUNT;
    }

    QBarSeries *series = new QBarSeries();
    QChart *chart = new QChart();
    QBarSet* bars = new QBarSet{"Densities"};
    bars->setColor(Qt::black);
    bars->append(QList<qreal>::fromVector(bins));
    series->append(bars);

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
    // Redraw histogram if data changed
    drawHistogram();
}
