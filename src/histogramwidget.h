#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include <QWidget>
#include "menuinterface.h"

class QVBoxLayout;
class MainWindow;
class Volume;

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

private:
    QVBoxLayout* mLayout{nullptr};
};

#endif // HISTOGRAMWIDGET_H
