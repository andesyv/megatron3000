#ifndef TRANSFERFUNCTIONWIDGET_H
#define TRANSFERFUNCTIONWIDGET_H

#include <QWidget>

namespace Ui {
class TransferFunctionWidget;
}

class TransferFunctionRenderer;

class TransferFunctionWidget : public QWidget
{
    Q_OBJECT

public:
    TransferFunctionWidget(QWidget* parent = nullptr);
    ~TransferFunctionWidget();

protected:
    TransferFunctionRenderer* mRenderer{nullptr};

private:
    Ui::TransferFunctionWidget *ui;
};

#endif // TRANSFERFUNCTIONWIDGET_H