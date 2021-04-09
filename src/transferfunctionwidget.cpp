#include "transferfunctionwidget.h"
#include "ui_nodepropertywidget.h"
#include "transferfunctionrenderer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
// #include <QColorDialog>

// class ColorLabel : public QLabel {
// public:
//     ColorLabel(QWidget* parent) : QLabel{parent} {}

// protected:
//     void paintEvent(QPaintEvent* e) override {

//     }
// };

TransferFunctionWidget::TransferFunctionWidget(QWidget* parent)
    : QWidget{parent},
    propertyUi{std::make_unique<Ui::NodePropertyWidget>()}
{

    // Layout:
    auto layout = new QVBoxLayout{this};
    layout->setContentsMargins(0, 0, 0, 0);

    // OpenGL Render Widget:
    mRenderer = new TransferFunctionRenderer{this};
    layout->addWidget(mRenderer);
    connect(mRenderer, &TransferFunctionRenderer::nodeSelected, this, &TransferFunctionWidget::select);
    
    // Proptery widget:
    // auto propertyWidget = new QWidget{this};
    // propertyUi->setupUi(propertyWidget);
    // // propertyWidget->setMaximumSize(QSize{QWIDGETSIZE_MAX, 100});
    // layout->addWidget(propertyWidget);
    // // Right side panel:
    // auto rightSide = new QWidget{this};
    // auto rightLayout = new QVBoxLayout{this};
    // rightSide->setLayout(rightLayout);
    // layout->addWidget(rightSide);

    setLayout(layout);
}

TransferFunctionWidget::~TransferFunctionWidget() = default;

void TransferFunctionWidget::select(Node& node) {
    auto widget = new QWidget{this};
    propertyUi->setupUi(widget);
    propertyUi->info->setText(QString{"Node: "}.append(QString::number(mRenderer->getNodeIndex(node))));
    // propertyWidget->setMaximumSize(QSize{QWIDGETSIZE_MAX, 100});
    
    if (mPropertyWidget) {
        auto layoutItem = layout()->replaceWidget(mPropertyWidget, widget);
        // Hide old so it doesn't render twice
        mPropertyWidget->hide();
        delete layoutItem; // Deleting layoutItem should also delete old widget
    } else
        layout()->addWidget(widget);

    mPropertyWidget = widget;

    qDebug() << "Node selected!";
}

void TransferFunctionWidget::deselect() {
    if (mPropertyWidget == nullptr)
        return;
    
    layout()->removeWidget(mPropertyWidget);
    // Note: According to the Qt documentation the layout will do cleanup for us.
    mPropertyWidget = nullptr;
}