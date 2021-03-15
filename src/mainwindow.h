#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <vector>
#include <QMatrix4x4>
#include <QString>
#include <QDockWidget>
#include <QCloseEvent>

class QWidget;
class Volume;
class DockWrapper;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    void addWidget(DockWrapper* widget);

    // Global camera matrix for all render-widgets
    QMatrix4x4 mGlobalViewMatrix;
    std::unique_ptr<Volume> mGlobalVolume;
    std::vector<QDockWidget*> mWidgets;

    ~MainWindow();

private:
    std::unique_ptr<Ui::MainWindow> mUi;

    DockWrapper* createWrapperWidget(QWidget* widget, const QString& title = "Dockwidget");
    // ALgorithm for finding new dock widget placements.
    void layoutDockWidget(DockWrapper* newWidget);
};





/**
 * @brief Wrapper helper class for QDockWidget
 * 
 */
class DockWrapper : public QDockWidget {
public:
    explicit DockWrapper(const QString &title, QWidget *parent = nullptr,
                         Qt::WindowFlags flags = Qt::WindowFlags())
        : QDockWidget{title, parent, flags} {}

protected:
    void closeEvent(QCloseEvent* event) override {
        auto parent = dynamic_cast<MainWindow*>(parentWidget());
        if (parent) {
            auto& widgets = parent->mWidgets;
            auto pos = std::find(widgets.begin(), widgets.end(), this);
            if (pos != widgets.end())
                widgets.erase(pos);
        }
        event->accept();
    }
};

#endif // MAINWINDOW_H
