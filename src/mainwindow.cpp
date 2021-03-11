#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "viewport2d.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), mUi{new Ui::MainWindow}
{
    mUi->setupUi(this);
    
    // Connections:
    connect(mUi->action2D_Viewport, &QAction::triggered, this, [&](){
        // Lambda mediator: button connected to lambda -> lambda decides what widget to add to viewport
        addWidget(new Viewport2D{this});
    });

    mGlobalViewMatrix.setToIdentity();
}

void MainWindow::addWidget(QWidget* widget) {
    std::cout << "Widget added!" << std::endl;
    mWidgets.emplace_back(widget);
    mUi->ViewportLayout->addWidget(widget);
}

MainWindow::~MainWindow() = default;

