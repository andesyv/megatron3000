#include "histogramwidget.h"
#include <QtCharts>
#include <QVBoxLayout>
#include "volume.h"
#include "mainwindow.h"
#include <QChartView>
#include "math.h"

#include <QtConcurrentRun>

using namespace QtCharts;

HistogramWidget::HistogramWidget(QWidget *parent) :
    QWidget(parent),
    IMenu{this},
    mRunner{std::make_unique<HistogramRunner>()}
{
    // Layout:
    mLayout = new QVBoxLayout{this};
    mLayout->setContentsMargins(0, 0, 0, 0);

    auto tfMapAction = mViewMenu->addAction("Map to transfer function");
    tfMapAction->setCheckable(true);

    mLayout->addWidget(mMenuBar);

    createView();

    // Connect future watcher (concurrency)
    connect(&mWatcher, &QFutureWatcher<std::vector<qreal>>::finished, this, &HistogramWidget::finishHistogramGeneration);
    connect(tfMapAction, &QAction::toggled, [&](bool checked){
        mMapToTransferFunction = checked;
        drawHistogram();
    });

    // Render histogram if we have data for it:
    if (mVolume) {
        drawHistogram();
        mTransferFunctionWatcher = connect(mVolume.get(), &Volume::transferFunctionUpdated, this, [&](){
            if (mMapToTransferFunction)
                drawHistogram();
        });
    }
}

void HistogramWidget::drawHistogram()
{
    if (!mVolume)
        return;
    
    // Initialize async func:

    const auto tfValues = mMapToTransferFunction ? mVolume->transferFunctionValues() : std::vector<QVector4D>{};

    // Note to self: There's apparantly no easy way to pass gui objects between threads,
    // so can only really manipulate widgets with other threads.
    
    auto oldRunner = std::move(mRunner);
    auto oldFuture = std::move(mFuture);

    // Make new runner
    mRunner = std::make_unique<HistogramRunner>(mVolume, tfValues);
    
    // Assign new future
    mFuture = mRunner->future();
    mWatcher.setFuture(mFuture);

    // Mark last runner for stopping:
    oldRunner->mCancelled = true;

    // Start new thread (add it to queue)
    QThreadPool::globalInstance()->start(mRunner.get());

    // Catch up to other thread by just waiting
    // We need to destroy old runner after it's finished
    oldFuture.waitForFinished();
}

HistogramWidget::~HistogramWidget() = default;

std::shared_ptr<Volume> HistogramWidget::getVolume() {
    return mVolume;
}

void HistogramWidget::volumeSwitched() {
    // Redraw histogram if data changed
    drawHistogram();
    
    disconnect(mTransferFunctionWatcher);
    mTransferFunctionWatcher = connect(mVolume.get(), &Volume::transferFunctionUpdated, this, [&](){
        if (mMapToTransferFunction)
            drawHistogram();
    });
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





/* 
 * QFutureInterface is an udocumented class for interacting with
 * QFutures from Qt5, replaced by QPromise in Qt6. In order
 * for it to be able to send signals across threads, it needs
 * to be declared as a metatype.
 * See https://stackoverflow.com/questions/59197694/qt-how-to-create-a-qfuture-from-a-thread
 */
Q_DECLARE_METATYPE( QFutureInterface<QVector<qreal>> );

HistogramRunner::HistogramRunner(std::shared_ptr<Volume> volume, std::vector<QVector4D> tfValues)
    : QRunnable{}, mVolume{volume}, mTfValues{tfValues} {
    // Need to call this line once, hence why it's static.
    static const auto typeId{qRegisterMetaType<QFutureInterface<QVector<qreal>>>()};

    setAutoDelete(false);
    // Initiate future:
    mFutureInterface.reportStarted();
}

void HistogramRunner::run() {
    // If volume is empty, exit early.
    if (!mVolume || mCancelled) {
        mFutureInterface.reportResult({});
        mFutureInterface.reportFinished();
        return;
    }
    
    const unsigned int BIN_COUNT = 256;
    const unsigned int VALUE_COUNT = static_cast<unsigned int>(mVolume->data().size());

    // Note: Using QVector because QList is apparantly a linked list and doesn't have random access.
    // Not sure why Qt decided it was a good idea to therefore implement everything using QLists...
    QVector<qreal> bins;
    bins.resize(BIN_COUNT);

    if (mCancelled) {
        mFutureInterface.reportResult(bins);
        mFutureInterface.reportFinished();
        return;
    }

    // Inject data into bins:
    for (auto value : mVolume->data()) {
        if (mCancelled) {
            mFutureInterface.reportResult(bins);
            mFutureInterface.reportFinished();
            return;
        }
        // Data should already be normalized so index can be found with floor(value * BIN_COUNT)
        // (clamp just in case)
        const int binIndex = std::clamp(static_cast<int>(value * BIN_COUNT), 0, static_cast<int>(BIN_COUNT) - 1);
        // Increment bins voxel count
        // Sum of all bins should be 1, so divide by value count.
        bins[binIndex] += 1.0 / VALUE_COUNT;
    }

    if (!mTfValues.empty()) {
        const auto SIZE = bins.size();
        for (int i{0}; i < SIZE; ++i) {
            if (mCancelled) {
                mFutureInterface.reportResult(bins);
                return;
            }
            const auto t = i / static_cast<double>(SIZE - 1);
            const auto a = megamath::piecewiseLerp(mTfValues, t).w();
            bins[i] *= a;
        }
    }

    mFutureInterface.reportResult(bins);
    mFutureInterface.reportFinished();
}

QFuture<QVector<qreal>> HistogramRunner::future() {
    return mFutureInterface.future();
}