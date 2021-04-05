#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QDockWidget>
#include "volume.h"
#include "shaders/shadermanager.h"
#include <QShortcut>

// Modules:
#include "viewport2d.h"
#include "viewport3d.h"
#include "datawidget.h"
#include "histogramwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    mUi{new Ui::MainWindow},
    mGlobalVolume{std::make_shared<Volume>()}
{
    mUi->setupUi(this);
    // QMainWindow requires a cental widget but we don't use it.
    mUi->centralwidget->hide();
    
    // Connections:
    connect(mUi->action2D_Viewport, &QAction::triggered, this, [&](){
        // Lambda mediator: button connected to lambda -> lambda decides what widget to add to viewport
        addWidget(createWrapperWidget(new Viewport2D{this}, "2D Viewport"));
    });
    connect(mUi->action3D_Viewport, &QAction::triggered, this, [&](){
        addWidget(createWrapperWidget(new Viewport3D{this}, "3D Viewport"));
    });
    connect(mUi->actionHistogram_Widget, &QAction::triggered, this, [&](){
        addWidget(createWrapperWidget(new HistogramWidget{this}, "Histogram Widget"));
    });
    connect(mUi->actionOpen, &QAction::triggered, this, &MainWindow::load);

    // Manually create a rendering widget:
    // NOTE: For datawidget to be able to create a volume, a render widget must be present. Else OpenGL crashes.
    mUi->action2D_Viewport->trigger();
    //mUi->actionHistogram_Widget->trigger();

    loadData();

    mGlobalViewMatrix.setToIdentity();

    auto shortcut = new QShortcut{QKeySequence{tr("F5", "Reload_Shaders")}, this};
    mShortcuts.push_back(shortcut);
    connect(shortcut, &QShortcut::activated, this, [](){
        std::cout << "Reloading shaders!" << std::endl;
        ShaderManager::get().reloadShaders();
    });
}

void MainWindow::addWidget(DockWrapper* widget) {
#ifndef NDEBUG
    std::cout << "Widget added!" << std::endl;
#endif

    // If the widget has a menu, add dock controls to the menu
    // if (auto* menuwidget = dynamic_cast<IMenu*>(widget)) {
    //     auto action = menuwidget->mViewMenu->addAction("Toggle window bar");
    //     action->setCheckable(true);
    //     connect(action, &QAction::toggled, dock, [&](bool checked){
            
    //     });
    // }

    layoutDockWidget(widget);
    mWidgets.push_back(widget);
}

DataWidget* MainWindow::loadData(Volume* targetVolume) {
    auto widget = new DataWidget{targetVolume != nullptr ? targetVolume : mGlobalVolume.get(), this};

    widget->setWindowFlag(Qt::Window);
    widget->show();
    return widget;
}

void MainWindow::load() {
    loadData();
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
