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
class QShortcut;
class DataWidget;
class IMenu;



/**
 * @brief Wrapper helper class for QDockWidget
 * 
 */
class DockWrapper : public QDockWidget {
public:
    explicit DockWrapper(
        const QString &title,
        QWidget *parent = nullptr,
        Qt::WindowFlags flags = Qt::WindowFlags()
    );

protected:
    void closeEvent(QCloseEvent* event) override;
};


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief Main application wrapper
 * Handles all widgets, global shortcuts, and volumes.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    void addWidget(DockWrapper* widget);

    // Global camera matrix for all render-widgets
    QMatrix4x4 mGlobalViewMatrix;
    std::vector<QDockWidget*> mWidgets;
    std::vector<QShortcut*> mShortcuts;

    auto volumesList() const { return mVolumes; }

    ~MainWindow();

signals:
    void loaded(std::shared_ptr<Volume>);
    void volumesUpdated(const std::vector<std::pair<std::string, std::shared_ptr<Volume>>>& volumeList);

public slots:
    /**
     * @brief Open up a data loading popup window
     * Opens up a data widget as a data loading popup window.
     * The resulting volume loaded from this operation can
     * be fetched by hooking up to the DataWidget::loaded signal.
     *
     * @return DataWidget* A pointer to the created widget
     */
    DataWidget* loadData();

    // Helper slot for button.
    void load(bool bOpenLast = false);

private:
    std::unique_ptr<Ui::MainWindow> mUi;

    DockWrapper* createWrapperWidget(QWidget* widget, const QString& title = "Dockwidget");

    /**
     * @brief Create a wrapped Widget object
     * This works the same as createWrapperWidget, except that it defers the
     * creation of the widget until after the wrapper dock is created.
     * @tparam T The type of widget. Must accept the pattern T{QWidget* parent}
     * @param title The title of the widget
     * @return DockWrapper* pointer to new wrapped widget
     */
    template <typename T>
    DockWrapper* createWrappedWidget(const QString& title = "Dockwidget") {
        auto dock = new DockWrapper{title, this};
        dock->setWidget(new T{dock});
        return dock;
    }
    // ALgorithm for finding new dock widget placements.
    void layoutDockWidget(DockWrapper* newWidget);

    std::vector<std::pair<std::string, std::shared_ptr<Volume>>> mVolumes;
};

#endif // MAINWINDOW_H
