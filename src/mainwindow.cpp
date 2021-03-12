#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QDockWidget>
#include "volume.h"

// Modules:
#include "viewport2d.h"
#include "viewport3d.h"
#include "datawidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    mUi{new Ui::MainWindow},
    mGlobalVolume{std::make_unique<Volume>()}
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
    connect(mUi->actionData_Manager, &QAction::triggered, this, [&](){
        addWidget(createWrapperWidget(new DataWidget{mGlobalVolume.get(), this}, "Data Manager"));
    });
    mUi->actionData_Manager->trigger();

    mGlobalViewMatrix.setToIdentity();
}

void MainWindow::addWidget(QDockWidget* widget) {
    std::cout << "Widget added!" << std::endl;

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

QDockWidget* MainWindow::createWrapperWidget(QWidget* widget, const QString& title) {
    auto dock = new QDockWidget{title, this};
    dock->setAttribute(Qt::WA_DeleteOnClose);
    dock->setWidget(widget);
    return dock;
}

// Inspired by JezzBall :)
void MainWindow::layoutDockWidget(QDockWidget* newWidget) {
    bool bHorizontal{true};
    int largestAxis{0};
    QDockWidget* splitWidget{nullptr};

    // 1. Find largest axis, and orientation.
    for (auto widget : mWidgets) {
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
