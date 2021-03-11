#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <vector>
#include <QMatrix4x4>

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

    // Global camera matrix for all render-widgets
    QMatrix4x4 mGlobalViewMatrix;

    ~MainWindow();

private:
    std::unique_ptr<Ui::MainWindow> mUi;
    std::vector<QWidget*> mWidgets;
};
#endif // MAINWINDOW_H
