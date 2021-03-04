#ifndef TRANSFERFUNCTIONWIDGET_H
#define TRANSFERFUNCTIONWIDGET_H

#include <QWidget>

namespace Ui {
class TransferFunctionWidget;
}

class TransferFunctionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TransferFunctionWidget(QWidget *parent = nullptr);
    ~TransferFunctionWidget();

private:
    Ui::TransferFunctionWidget *ui;
};

#endif // TRANSFERFUNCTIONWIDGET_H
