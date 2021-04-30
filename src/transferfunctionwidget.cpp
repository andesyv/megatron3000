#include "transferfunctionwidget.h"
#include "ui_nodepropertywidget.h"
#include "transferfunctionrenderer.h"
#include <QVBoxLayout>
#include <QAbstractButton>
#include <QPaintEvent>
#include <QPainter>
#include <QColorDialog>
#include <QMenuBar>

class ColorButton : public QAbstractButton {
public:
    explicit ColorButton(QWidget* parent = nullptr) : QAbstractButton{parent} {}
    // explicit QAbstractButton(const QString &text, QWidget *parent = nullptr) : QAbstractButton{text, parent} {}
    explicit ColorButton(const QColor& color, const QString& texxxxxxt, QWidget *parent = nullptr)
        : QAbstractButton{parent}, mColor{color} {
            setText(texxxxxxt);
        }

    QSize sizeHint() const override {
        return {50, 30};
    }

    void setColor(const QColor& col) {
        mColor = col;
        update();
    }

    auto color() const {
        return mColor;
    }

protected:
    void paintEvent(QPaintEvent* e) override {
        // QPushButton::paintEvent(e);
        const auto rect = e->rect();
        QPainter painter{this};
        painter.fillRect(rect, mColor);
        painter.drawText(rect, Qt::AlignCenter, text());
    }

    QColor mColor;
};

TransferFunctionWidget::TransferFunctionWidget(QWidget* parent)
    : QWidget{parent},
    IMenu{this},
    propertyUi{std::make_unique<Ui::NodePropertyWidget>()}
{
    // Layout:
    auto layout = new QVBoxLayout{this};
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(mMenuBar);

    // OpenGL Render Widget:
    mRenderer = new TransferFunctionRenderer{this};
    if (mVolume)
        mRenderer->mVolume = mVolume;
    layout->addWidget(mRenderer);
    connect(mRenderer, &TransferFunctionRenderer::nodeSelected, this, &TransferFunctionWidget::select);

    setLayout(layout);
}

TransferFunctionWidget::~TransferFunctionWidget() = default;

void TransferFunctionWidget::select(Node& node) {
    mSelectedNode = &node;

    auto widget = new QWidget{this};
    propertyUi->setupUi(widget);
    propertyUi->info->setText(QString{"Node: "}.append(QString::number(mRenderer->getSortedNodeIndex(node))));

    // Swap old button for custom button
    mColorButton = new ColorButton{node.color, "Color", widget};
    auto layoutItem = widget->layout()->replaceWidget(propertyUi->color, mColorButton);
    propertyUi->color->hide();
    delete layoutItem;
    connect(mColorButton, &QAbstractButton::clicked, this, &TransferFunctionWidget::pickColor);
    
    if (mPropertyWidget) {
        auto layoutItem = layout()->replaceWidget(mPropertyWidget, widget);
        // Hide old so it doesn't render twice
        mPropertyWidget->hide();
        delete layoutItem; // Deleting layoutItem should also delete old widget
    } else
        layout()->addWidget(widget);

    mPropertyWidget = widget;
}

void TransferFunctionWidget::deselect() {
    if (mPropertyWidget == nullptr)
        return;
    
    layout()->removeWidget(mPropertyWidget);
    // Note: According to the Qt documentation the layout will do cleanup for us.
    mPropertyWidget = nullptr;
    mSelectedNode = nullptr;
    mColorButton = nullptr;
}

void TransferFunctionWidget::volumeSwitched() {
    mRenderer->mVolume = mVolume;
    mRenderer->nodesChanged();
}

void TransferFunctionWidget::pickColor() {
    // Note: QColorDialog gives a debug warning about failing to set correct size, but it seems safe to ignore it.
    const auto newColor = QColorDialog::getColor(mSelectedNode->color, this, "Color! :o");
    if (!newColor.isValid())
        return;

    mColorButton->setColor(newColor);
    
    mSelectedNode->color = newColor;
    mRenderer->nodesChanged();
}