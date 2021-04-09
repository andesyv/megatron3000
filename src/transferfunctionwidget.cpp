#include "transferfunctionwidget.h"
#include "ui_transferfunctionwidget.h"
#include "transferfunctionrenderer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

TransferFunctionWidget::TransferFunctionWidget(QWidget* parent)
    : QWidget{parent},
    ui(new Ui::TransferFunctionWidget)
{
    ui->setupUi(this);

    // // Layout:
    // auto layout = new QVBoxLayout{this};
    // layout->setContentsMargins(0, 0, 0, 0);

    // OpenGL Render Widget:
    mRenderer = new TransferFunctionRenderer{this};
    auto old = layout()->replaceWidget(ui->RenderWidget, mRenderer);
    if (old)
        delete old;
    
    // // Right side panel:
    // auto rightSide = new QWidget{this};
    // auto rightLayout = new QVBoxLayout{this};
    // rightSide->setLayout(rightLayout);
    // layout->addWidget(rightSide);

    // setLayout(layout);
}

TransferFunctionWidget::~TransferFunctionWidget()
{
    delete ui;
};
