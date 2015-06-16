#include "borderrect.h"

borderRect::borderRect(qreal x, qreal y, qreal w, qreal h, QGraphicsItem * parent)
    : QGraphicsRectItem(x, y, w, h, parent)
{
    // so far nothing needed here
    size.setWidth(w);
    size.setHeight(h);
}

borderRect::~borderRect()
{

}

void borderRect::setSize(QSizeF n_size)
{
    QPointF pos = this->pos();
    size = n_size;
    this->setRect(pos.x(), pos.y(), size.width(), size.height());
}

QSizeF borderRect::getSize()
{
    return size;
}

