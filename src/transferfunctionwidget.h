#ifndef TRANSFERFUNCTIONWIDGET_H
#define TRANSFERFUNCTIONWIDGET_H

#include <QWidget>
#include <memory>

namespace Ui {
class NodePropertyWidget;
}

class TransferFunctionRenderer;
class QColorDialog;
class ColorButton;
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
    QColorDialog* mColorDialog{nullptr};
    Node* mSelectedNode{nullptr};
    ColorButton* mColorButton;

    void select(Node& node);
    void deselect();

protected slots:
    void pickColor();

private:
    std::unique_ptr<Ui::NodePropertyWidget> propertyUi;

};

#endif // TRANSFERFUNCTIONWIDGET_H