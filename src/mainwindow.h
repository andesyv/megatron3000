#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <vector>
#include <QMatrix4x4>
#include <QString>

class QWidget;
class QDockWidget;
class Volume;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    void addWidget(QDockWidget* widget);

    // Global camera matrix for all render-widgets
    QMatrix4x4 mGlobalViewMatrix;
    std::unique_ptr<Volume> mGlobalVolume;

    ~MainWindow();

private:
    std::unique_ptr<Ui::MainWindow> mUi;
    std::vector<QDockWidget*> mWidgets;

    QDockWidget* createWrapperWidget(QWidget* widget, const QString& title = "Dockwidget");
    // ALgorithm for finding new dock widget placements.
    void layoutDockWidget(QDockWidget* newWidget);
};
#endif // MAINWINDOW_H
