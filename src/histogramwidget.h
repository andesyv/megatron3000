#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include <QWidget>
#include "menuinterface.h"
#include <QFutureWatcher>
#include <QFuture>

class QVBoxLayout;
class MainWindow;
class Volume;

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
    void volumeSwitched() override final;
    void createView();
    static QVector<qreal> generateHistogram(std::shared_ptr<Volume> volume, std::vector<QVector4D> tfValues);

private:
    QVBoxLayout* mLayout{nullptr};
    bool mMapToTransferFunction{false};
    QtCharts::QChartView* mChartView{nullptr};
    QtCharts::QChart* mChart{nullptr};
    QFuture<QVector<qreal>> mFuture;
    QFutureWatcher<QVector<qreal>> mWatcher;
    QMetaObject::Connection mTransferFunctionWatcher;

private slots:
    void finishHistogramGeneration();
};

#endif // HISTOGRAMWIDGET_H
