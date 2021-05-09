#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDockWidget>
#include <QDebug>
#include "volume.h"
#include "shaders/shadermanager.h"
#include <QShortcut>
#include <QApplication>

// Modules:
#include "viewport2d.h"
#include "viewport3d.h"
#include "datawidget.h"
#include "histogramwidget.h"
#include "transferfunctionwidget.h"

DockWrapper::DockWrapper(const QString &title, QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget{title, parent, flags}
{
    setFocusPolicy(Qt::StrongFocus);
}

void DockWrapper::closeEvent(QCloseEvent* event) {
    auto parent = dynamic_cast<MainWindow*>(parentWidget());
    if (parent) {
        auto& widgets = parent->mWidgets;
        auto pos = std::find(widgets.begin(), widgets.end(), this);
        if (pos != widgets.end())
            widgets.erase(pos);
    }
    event->accept();
}






MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    mUi{new Ui::MainWindow}
{
    mUi->setupUi(this);
    // QMainWindow requires a cental widget but we don't use it.
    mUi->centralwidget->hide();
    
    // Connections:
    connect(mUi->action2D_Viewport, &QAction::triggered, this, [&](){
        // Lambda mediator: button connected to lambda -> lambda decides what widget to add to viewport
        addWidget(createWrappedWidget<Viewport2D>("2D Viewport"));
    });
    connect(mUi->action3D_Viewport, &QAction::triggered, this, [&](){
        addWidget(createWrappedWidget<Viewport3D>("3D Viewport"));
    });
    connect(mUi->actionTransfer_function, &QAction::triggered, this, [&](){
        addWidget(createWrappedWidget<TransferFunctionWidget>("Transfer function"));
    });
    connect(mUi->actionHistogram_Widget, &QAction::triggered, this, [&](){
        addWidget(createWrappedWidget<HistogramWidget>("Histogram Widget"));
    });
    connect(mUi->actionOpen, &QAction::triggered, this, &MainWindow::load);
    connect(mUi->actionOpen_last_opened, &QAction::triggered, this, [=](){ load(true); });

    // Manually create a rendering widget:
    // NOTE: For datawidget to be able to create a volume, a render widget must be present. Else OpenGL crashes.
    mUi->action2D_Viewport->trigger();
    //mUi->actionHistogram_Widget->trigger();

    // loadData();

    mGlobalViewMatrix.setToIdentity();

    auto shortcut = new QShortcut{QKeySequence{tr("F5", "Reload_Shaders")}, this};
    mShortcuts.push_back(shortcut);
    connect(shortcut, &QShortcut::activated, this, [](){
        qDebug() << "Reloading shaders!";
        ShaderManager::get().reloadShaders();
    });
    shortcut = new QShortcut{QKeySequence{tr("Ctrl+W", "Close Viewport")}, this};
    mShortcuts.push_back(shortcut);
    connect(shortcut, &QShortcut::activated, this, [](){
        auto focus = QApplication::focusWidget();
        if (focus)
            focus->close();
    });
}

void MainWindow::addWidget(DockWrapper* dock) {
#ifndef NDEBUG
    qDebug() << "Widget added!";
#endif

    // If the widget has a menu, add dock controls to the menu
    if (auto* menuwidget = dynamic_cast<IMenu*>(dock->widget())) {
        auto action = menuwidget->mViewMenu->addAction("Toggle window bar");
        action->setCheckable(true);

        connect(action, &QAction::triggered, dock, [=](bool checked){
            if (dock->isFloating()) {
                action->setChecked(false);
                return;
            }

            if (checked)
                dock->setTitleBarWidget(new QWidget{this});
            else {
                auto old = dock->titleBarWidget();
                dock->setTitleBarWidget(nullptr);
                if (old)
                    delete old;
            }
        });
    }

    layoutDockWidget(dock);
    mWidgets.push_back(dock);
    dock->setFocus();
}

DataWidget* MainWindow::loadData() {
    auto widget = new DataWidget{this};
    widget->setWindowFlag(Qt::Window);
    widget->show();
    connect(widget, &DataWidget::loaded, this, [&](std::shared_ptr<Volume> volume, const std::string& identifier){
        // Make sure identifier is unique:
        auto modifiedIdentifier{identifier};
        for (int i{2}; std::any_of(mVolumes.begin(), mVolumes.end(), [=](const auto& vol){
            return vol.first == modifiedIdentifier;
        }); ++i)
            modifiedIdentifier = identifier + std::to_string(i);

        mVolumes.emplace_back(modifiedIdentifier, volume);

        volumesUpdated(mVolumes);
    });
    return widget;
}

void MainWindow::load(bool bOpenLast) {
    auto widget = loadData();
    connect(widget, &DataWidget::loaded, this, [&](std::shared_ptr<Volume> volume){
        if (1 < mVolumes.size())
            mVolumes.erase(mVolumes.begin());

        // If we have multiple volumes, swap so global is first.
        if (1 < mVolumes.size())
            std::swap(mVolumes.front(), mVolumes.back());

        // Mitigate volume onwards to whatever widgets wants it
        loaded(volume);
    });
    
    if (bOpenLast)
        widget->loadCached();
}

DockWrapper* MainWindow::createWrapperWidget(QWidget* widget, const QString& title) {
    auto dock = new DockWrapper{title, this};
    /** This destroys widget and sub-widgets when closing the wrapper,
     * freeing used resources. But a current bug with QOpenGLWidget
     * makes the mainwindow render wrong when they're destroyed.
     */
    // dock->setAttribute(Qt::WA_DeleteOnClose);
    dock->setWidget(widget);
    return dock;
}

// Inspired by JezzBall :)
void MainWindow::layoutDockWidget(DockWrapper* newWidget) {
    bool bHorizontal{true};
    int largestAxis{0};
    QDockWidget* splitWidget{nullptr};

    // 1. Find largest axis, and orientation.
    for (auto widget : mWidgets) {
        if (!widget->isVisible())
            continue;
        
        const auto& s = widget->size();
        if (largestAxis < s.width()) {
            largestAxis = s.width();
            bHorizontal = true;
            splitWidget = widget;
        }
        if (largestAxis < s.height()) {
            largestAxis = s.height();
            bHorizontal = false;
            splitWidget = widget;
        }
    }

    // 2. Split largest widget on axis and add new widget
    if (splitWidget == nullptr) {
        // If we didn't find any widgets, assume we have no widget from before and just add it normally.
        addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, newWidget, Qt::Horizontal);
    } else {
        splitDockWidget(splitWidget, newWidget, bHorizontal ? Qt::Horizontal : Qt::Vertical);

        // 3. Resize both widgets to equal size
        resizeDocks(
            {splitWidget, newWidget},
            {largestAxis / 2, largestAxis / 2},
            bHorizontal ? Qt::Horizontal : Qt::Vertical
        );
    }
}

MainWindow::~MainWindow() = default;
