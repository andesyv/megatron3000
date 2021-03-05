#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <vector>

class QWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    void addWidget(QWidget* widget);

    ~MainWindow();

private:
    std::unique_ptr<Ui::MainWindow> mUi;
    std::vector<std::unique_ptr<QWidget>> mWidgets;
};
#endif // MAINWINDOW_H
