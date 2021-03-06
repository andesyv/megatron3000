#include "histogramwidget.h"
#include <QtCharts>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QWidgetAction>
#include "volume.h"
#include "mainwindow.h"
#include <QChartView>
#include "math.h"

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
    
    // Bin customization menu widget:
    const auto binSizeContainerLayout = new QHBoxLayout{};
    binSizeContainerLayout->setContentsMargins(0, 0, 0, 0);
    const auto label = new QLabel{"Bins:"};
    label->setAlignment(Qt::AlignCenter);
    binSizeContainerLayout->addWidget(label);
    auto spinner = new QSpinBox{this};
    spinner->setRange(10, 256);
    spinner->setValue(mBinCount);
    binSizeContainerLayout->addWidget(spinner);
    const auto binSizeContainerWidget = new QWidget{this};
    binSizeContainerWidget->setLayout(binSizeContainerLayout);
    const auto binSizeAction = new QWidgetAction{this};
    binSizeAction->setDefaultWidget(binSizeContainerWidget);
    mViewMenu->addAction(binSizeAction);

    auto sourceMenu = mViewMenu->addMenu("Histogram source: ");
    mSourceOptionActions = {
        new QAction{"All volume", this},
        new QAction{"Cutting plane aligned", this}
    };
    for (auto action : mSourceOptionActions) {
        action->setCheckable(true);
        if (action == mSourceOptionActions.front())
            action->setChecked(true);
        
        connect(action, &QAction::triggered, this, [&, action](bool bChecked){
            for (auto a : mSourceOptionActions)
                if (a != action)
                    a->setChecked(false);

            const auto sourceActionPos = std::find(mSourceOptionActions.begin(), mSourceOptionActions.end(), action);
#ifndef NDEBUG
            if (sourceActionPos == mSourceOptionActions.end())
                throw std::logic_error{"Action not in list. >:("};
#endif
            const auto index = sourceActionPos - mSourceOptionActions.begin();
            mHistogramSource = static_cast<HistogramSource>(index);
            drawHistogram();
        });
    }
    sourceMenu->addActions(mSourceOptionActions); 

    mLayout->addWidget(mMenuBar);

    createView();

    // Connect future watcher (concurrency)
    connect(&mWatcher, &QFutureWatcher<std::vector<qreal>>::finished, this, &HistogramWidget::finishHistogramGeneration);
    connect(tfMapAction, &QAction::toggled, [&](bool checked){
        mMapToTransferFunction = checked;
        drawHistogram();
    });
    connect(spinner, qOverload<int>(&QSpinBox::valueChanged), this, [&](int val){
        mBinCount = val;
        if (mVolume)
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
    const auto& cuttingPlane = mHistogramSource == HistogramSource::Plane ? mVolume->m_slicingGeometry : Slicing::Plane{};

    // Note to self: There's apparantly no easy way to pass gui objects between threads,
    // so can only really manipulate widgets with other threads.
    
    auto oldRunner = std::move(mRunner);
    auto oldFuture = std::move(mFuture);

    // Make new runner
    mRunner = std::make_unique<HistogramRunner>(mBinCount, mHistogramSource, mVolume, tfValues, cuttingPlane);
    
    // Assign new future
    mFuture = mRunner->future();
    mWatcher.setFuture(mFuture);

    mRunner->mPrevRunner = std::move(oldRunner);
    mRunner->mPrevFuture = oldFuture;

    // Mark last runner for stopping:
    mRunner->mPrevRunner->mCancelled = true;

    // Start new thread (add it to queue)
    QThreadPool::globalInstance()->start(mRunner.get());
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

    auto bars = new QBarSet{"Density"};
    bars->setColor(Qt::black);
    bars->setBorderColor(Qt::black);
    auto barPen = bars->pen();
    barPen.setWidth(0);
    bars->setPen(barPen);
    bars->append(QList<qreal>::fromVector(bins));
       
    auto series = new QBarSeries();
    series->setBarWidth(1.0);
    series->setLabelsVisible(false);
    series->append(bars);

    chart->addSeries(series);
    auto yaxis = new QValueAxis{this};
    yaxis->setRange(*std::min_element(bins.begin(), bins.end()), *std::max_element(bins.begin(), bins.end()));
    chart->addAxis(yaxis, Qt::AlignLeft);
    
    auto xaxis = new QValueAxis{this};
    xaxis->setRange(0.0, 1.0);
    chart->addAxis(xaxis, Qt::AlignBottom);
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

HistogramRunner::HistogramRunner(
    unsigned int binCount,
    HistogramSource source,
    std::shared_ptr<Volume> volume,
    std::vector<QVector4D> tfValues,
    const Slicing::Plane& plane)
    : QRunnable{}, mBinCount{binCount}, mSource{source}, mVolume{volume}, mTfValues{tfValues} {
    // Need to call this line once, hence why it's static.
    static const auto typeId{qRegisterMetaType<QFutureInterface<QVector<qreal>>>()};

    setAutoDelete(false);
    // Initiate future:
    mFutureInterface.reportStarted();
}

void HistogramRunner::run() {
    // If a previous runner is still running,
    // wait for that one before starting to cleanup after it.
    if (mPrevRunner) {
        // Catch up to other thread by just waiting
        mPrevFuture.waitForFinished();
        // Delete old thread
        mPrevRunner.reset();
    }


    // If volume is empty, exit early.
    if (!mVolume || mCancelled) {
        mFutureInterface.reportResult({});
        mFutureInterface.reportFinished();
        return;
    }

    // Note: Using QVector because QList is apparantly a linked list and doesn't have random access.
    // Not sure why Qt decided it was a good idea to therefore implement everything using QLists...
    QVector<qreal> bins;
    bins.resize(mBinCount);

    if (mCancelled) {
        mFutureInterface.reportResult(bins);
        mFutureInterface.reportFinished();
        return;
    }

    std::size_t valueCount = 0;

    // Inject data into bins:
    const auto& data = mVolume->data();
    for (auto i{0}; i < data.size(); ++i) {
        const auto& value = data[i];

        if (mCancelled) {
            mFutureInterface.reportResult(bins);
            mFutureInterface.reportFinished();
            return;
        }

        if (mSource == HistogramSource::Plane) {
            const auto box = mVolume->getVoxelBounds(mVolume->getVoxelIndex(i));
            if (!megamath::boxPlaneIntersect(box.pos, box.size, mPlane.pos, mPlane.dir))
                continue;
        }

        // Data should already be normalized so index can be found with floor(value * BIN_COUNT)
        // (clamp just in case)
        const int binIndex = std::clamp(static_cast<int>(value * mBinCount), 0, static_cast<int>(mBinCount) - 1);
        // Increment bins voxel count
        bins[binIndex] += 1.0;
        ++valueCount;
    }

    // Sum of all bins should be 1, so divide by value count.
    for (auto& bin : bins)
        bin /= valueCount;

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