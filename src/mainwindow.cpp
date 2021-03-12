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

    mWidgets.push_back(widget);
    addDockWidget(Qt::RightDockWidgetArea, widget);
}

QDockWidget* MainWindow::createWrapperWidget(QWidget* widget, const QString& title) {
    auto dock = new QDockWidget{title, this};
    dock->setAttribute(Qt::WA_DeleteOnClose);
    dock->setWidget(widget);
    return dock;
}

MainWindow::~MainWindow() = default;
