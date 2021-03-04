#include "transferfunctionwidget.h"
#include "ui_transferfunctionwidget.h"

TransferFunctionWidget::TransferFunctionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransferFunctionWidget)
{
    ui->setupUi(this);
}

TransferFunctionWidget::~TransferFunctionWidget()
{
    delete ui;
}
