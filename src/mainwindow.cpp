#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "viewport2d.h"
#include <iostream>
#include <QDockWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), mUi{new Ui::MainWindow}
{
    mUi->setupUi(this);
    // QMainWindow requires a cental widget but we don't use it.
    mUi->centralwidget->hide();
    
    // Connections:
    connect(mUi->action2D_Viewport, &QAction::triggered, this, [&](){
        // Lambda mediator: button connected to lambda -> lambda decides what widget to add to viewport
        addWidget(new Viewport2D{this});
    });

    mGlobalViewMatrix.setToIdentity();
}

void MainWindow::addWidget(QWidget* widget) {
    std::cout << "Widget added!" << std::endl;

    auto dock = new QDockWidget{"Dockwidget", this};
    dock->setAttribute(Qt::WA_DeleteOnClose);
    dock->setWidget(widget);
    mWidgets.push_back(dock);
    addDockWidget(Qt::RightDockWidgetArea, dock);
}

MainWindow::~MainWindow() = default;
