#ifndef TRANSFERFUNCTIONWIDGET_H
#define TRANSFERFUNCTIONWIDGET_H

#include <QWidget>
#include <memory>

namespace Ui {
class NodePropertyWidget;
}

class TransferFunctionRenderer;
struct Node;

class TransferFunctionWidget : public QWidget
{
    Q_OBJECT

public:
    TransferFunctionWidget(QWidget* parent = nullptr);
    ~TransferFunctionWidget();

protected:
    TransferFunctionRenderer* mRenderer{nullptr};
    QWidget* mPropertyWidget{nullptr};

    void select(Node& node);
    void deselect();

private:
    std::unique_ptr<Ui::NodePropertyWidget> propertyUi;
};

#endif // TRANSFERFUNCTIONWIDGET_H