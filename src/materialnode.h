#ifndef MATERIALNODE_H
#define MATERIALNODE_H

#include <QGraphicsItem>

/**
 * @brief Transfer function node for rendering in Transfer function widget
 * Based off Qt elastic nodes exaxmple
 * @see https://doc.qt.io/qt-5/qtwidgets-graphicsview-elasticnodes-example.html
 */
class MaterialNode : public QGraphicsItem {
public:
    MaterialNode(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // MATERIALNODE_H