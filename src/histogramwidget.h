#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include <QWidget>
#include "menuinterface.h"
#include <QFutureWatcher>
#include <memory>
#include "volume.h"
#include "runners.h"
#include "renderutils.h"

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
    HistogramSource mHistogramSource;

private:
    QVBoxLayout* mLayout{nullptr};
    bool mMapToTransferFunction{false};
    QtCharts::QChartView* mChartView{nullptr};
    QtCharts::QChart* mChart{nullptr};
    QList<QAction*> mSourceOptionActions;
    QFuture<QVector<qreal>> mFuture;
    QFutureWatcher<QVector<qreal>> mWatcher;
    QMetaObject::Connection mTransferFunctionWatcher;
    std::unique_ptr<HistogramRunner> mRunner;

private slots:
    void finishHistogramGeneration();
};

#endif // HISTOGRAMWIDGET_H
