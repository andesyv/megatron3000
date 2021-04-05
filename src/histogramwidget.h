#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include <QWidget>

class QVBoxLayout;
class MainWindow;
class Volume;

class HistogramWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HistogramWidget(QWidget *parent = nullptr);
    ~HistogramWidget();
    std::shared_ptr<Volume> getVolume() const;
    std::shared_ptr<Volume> getVolume();
    void drawStdLineGraph();
protected:
    MainWindow* mMainWindow{nullptr};

private:
    QVBoxLayout* mLayout{nullptr};
};

#endif // HISTOGRAMWIDGET_H
