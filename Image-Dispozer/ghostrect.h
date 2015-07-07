#ifndef GHOSTRECT_H
#define GHOSTRECT_H

#include <QRectF>
#include <QPointF>
#include <QGraphicsItem>
#include <QWidget>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

class ghostRect : public QGraphicsItem
{
public:
    ghostRect(qreal width, qreal height, QPointF pos, QGraphicsItem *parent);
    ~ghostRect();

    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

protected:
    QRectF boundingRect() const;

private:
    QRectF g_rect;
    QPointF position;
};

#endif // GHOSTRECT_H
