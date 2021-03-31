#include "materialnode.h"

#include <QPainter>
#include <QStyleOption>
#include <QDebug>

MaterialNode::MaterialNode(QGraphicsItem *parent)
    : QGraphicsItem{parent} {
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    // setCacheMode(DeviceCoordinateCache); // Optimilization
}

QRectF MaterialNode::boundingRect() const {
    constexpr qreal outlineStroke = 2.0;
    return QRectF{-10 - outlineStroke, -10 - outlineStroke, 23 + outlineStroke, 23 + outlineStroke};
}

QPainterPath MaterialNode::shape() const {
    QPainterPath path;
    path.addEllipse(-10, -10, 20, 20);
    return path;
}

void MaterialNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
    painter->drawEllipse(-7, -7, 20, 20);

    QRadialGradient gradient(-3, -3, 10);
    if (option->state & QStyle::State_Sunken) {
        gradient.setCenter(3, 3);
        gradient.setFocalPoint(3, 3);
        gradient.setColorAt(1, QColor(Qt::yellow).lighter(120));
        gradient.setColorAt(0, QColor(Qt::darkYellow).lighter(120));
    } else {
        gradient.setColorAt(0, Qt::yellow);
        gradient.setColorAt(1, Qt::darkYellow);
    }
    painter->setBrush(gradient);

    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-10, -10, 20, 20);
}

/*
"This notification is the only reason why the nodes need to keep a pointer back to the GraphWidget.
Another approach could be to provide such notification using a signal; in such case, Node would
need to inherit from QGraphicsObject."
*/
QVariant MaterialNode::itemChange(GraphicsItemChange change, const QVariant &value) {
    // if (change == ItemPositionHasChanged) 
    
    return QGraphicsItem::itemChange(change, value);
}

void MaterialNode::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    update();
    QGraphicsItem::mousePressEvent(event);
}

void MaterialNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}