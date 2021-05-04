#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include <QWidget>
#include "menuinterface.h"
#include <QFutureWatcher>
#include <QFuture>
#include <QRunnable>
#include <memory>
#include <QFutureInterface>

class QVBoxLayout;
class MainWindow;
class Volume;
class HistogramRunner;

namespace QtCharts {
    class QChartView;
    class QChart;
}

class HistogramWidget : public QWidget, public IMenu
{
    Q_OBJECT

public:
    explicit HistogramWidget(QWidget *parent = nullptr);
    ~HistogramWidget();
    std::shared_ptr<Volume> getVolume();
    void drawHistogram();
protected:
    MainWindow* mMainWindow{nullptr};
    unsigned int mBinCount = 30;
    void volumeSwitched() override final;
    void createView();

private:
    QVBoxLayout* mLayout{nullptr};
    bool mMapToTransferFunction{false};
    QtCharts::QChartView* mChartView{nullptr};
    QtCharts::QChart* mChart{nullptr};
    QFuture<QVector<qreal>> mFuture;
    QFutureWatcher<QVector<qreal>> mWatcher;
    QMetaObject::Connection mTransferFunctionWatcher;
    std::unique_ptr<HistogramRunner> mRunner;

private slots:
    void finishHistogramGeneration();
};


/**
 * @brief Helper class to wrap the histogram generation in a separate thread
 * This class is used by the global QThreadPool, which puts these runners in
 * a queue for them to wait their turn in order for a thread to become avilable
 * for them. When a thread is available, the HistogramRunners generates
 * a histogram, which after finished is inserted into the widget.
 * 
 * HistogramRunners can be cancelled mid-generation by setting mCancelled = true,
 * which will early return the connected future. 
 */
class HistogramRunner : public QRunnable {
public:
    HistogramRunner(unsigned int binCount = 256, std::shared_ptr<Volume> volume = {}, std::vector<QVector4D> tfValues = {});
    ~HistogramRunner() {}

    void run() override;
    QFuture<QVector<qreal>> future();

    bool mCancelled{false};
    const unsigned int mBinCount{30};
    std::unique_ptr<HistogramRunner> mPrevRunner;
    QFuture<QVector<qreal>> mPrevFuture;

private:
    // https://stackoverflow.com/questions/59197694/qt-how-to-create-a-qfuture-from-a-thread
    QFutureInterface<QVector<qreal>> mFutureInterface;
    std::shared_ptr<Volume> mVolume;
    std::vector<QVector4D> mTfValues;
};

#endif // HISTOGRAMWIDGET_H
