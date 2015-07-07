#include "ghostrect.h"

ghostRect::ghostRect(qreal width, qreal height, QPointF pos, QGraphicsItem * parent)
    : QGraphicsItem(parent)
{
    g_rect.setTopLeft(QPointF(-width/2.0, height/2.0));
    g_rect.setBottomRight(QPointF(width/2.0, -height/2.0));

    this->setPos(pos);
}

ghostRect::~ghostRect()
{

}

void ghostRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setPen(QPen(QColor(130,130,130,255), 1.0));
    painter->setBrush(QBrush(QColor(178, 178, 178, 25)));
    painter->drawRect(g_rect);
}

QRectF ghostRect::boundingRect() const
{
    return g_rect;
}

