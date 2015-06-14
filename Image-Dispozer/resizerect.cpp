#include "resizerect.h"

resizeRect::resizeRect(qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent)
    : QGraphicsItem(parent), cornerSquareSize(5.0)
{
    xPos = x;
    yPos = y;
    width = w;
    height = h;

    topLeft.setX(-width/2.0);
    topLeft.setY(height/2.0);
    topRight.setX(width/2.0);
    topRight.setY(height/2.0);

    bottomLeft.setX(-width/2.0);
    bottomLeft.setY(-height/2.0);
    bottomRight.setX(width/2.0);
    bottomRight.setY(-height/2.0);

    resize = NONE;

    flipX = 1;
    flipY = -1;

    constantZIndex = 1;

    image = QPixmap(); // empty pixmap

    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);

    setPos(x, y);
    setAcceptHoverEvents(true);
}

resizeRect::~resizeRect()
{

}

void resizeRect::incrementRotation(qreal angle)
{
    setRotation(rotation()+angle);
}

void resizeRect::switchFlipX()
{
    (flipX>0.0) ? flipX = -1.0 : flipX = 1.0;
}

void resizeRect::switchFlipY()
{
    (flipY>0.0) ? flipY = -1.0 : flipY = 1.0;
}

void resizeRect::setPixmap(image_handler *handler)
{
    image = *handler;
}

void resizeRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(isSelected())
    {
        // if item is selected, draw also bordering with resize handlers
        painter->save();

        painter->setPen(QPen(QBrush(Qt::darkRed), 1.0, Qt::DotLine));
        painter->drawRect(QRectF(topLeft, bottomRight));

        // draw small squares to the corners and in the middle of borders
        qreal cssHalf = cornerSquareSize/2.0;

        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(QColor(Qt::darkGreen)));

        painter->drawRect(QRectF(topLeft.x()-cssHalf, topLeft.y()-cssHalf, cornerSquareSize, cornerSquareSize)); // topLeft square
        painter->drawRect(QRectF(topRight.x()-cssHalf, topRight.y()-cssHalf, cornerSquareSize, cornerSquareSize)); // topRight square
        painter->drawRect(QRectF(bottomLeft.x()-cssHalf, bottomLeft.y()-cssHalf, cornerSquareSize, cornerSquareSize)); // bottomLeft square
        painter->drawRect(QRectF(bottomRight.x()-cssHalf, bottomRight.y()-cssHalf, cornerSquareSize, cornerSquareSize)); // bottomLeft square

        painter->drawRect(QRectF(-cssHalf, height/2.0-cssHalf, cornerSquareSize, cornerSquareSize)); // top border
        painter->drawRect(QRectF(width/2.0-cssHalf, -cssHalf, cornerSquareSize, cornerSquareSize)); // right border
        painter->drawRect(QRectF(-cssHalf, -height/2.0-cssHalf, cornerSquareSize, cornerSquareSize)); // bottom border
        painter->drawRect(QRectF(-width/2.0-cssHalf, -cssHalf, cornerSquareSize, cornerSquareSize)); // left border

        painter->restore();
    }

    painter->save();
    painter->scale(flipX*1.0, flipY*1.0);
    painter->drawPixmap(topLeft.x(), -topLeft.y(), width, height, image);
    painter->restore();
}

QRectF resizeRect::boundingRect() const
{
    // subtrackt full cornerSquareSize (not only half) will ensure better repainting while changing size of widget
    QRectF r((topLeft.x()-cornerSquareSize), -(topLeft.y()+cornerSquareSize), width+cornerSquareSize*2.0, height+cornerSquareSize*2.0);
    return r;
}


bool resizeRect::isNearAt(QPointF pos, QPointF ref)
{
    qreal dx = pos.x()-ref.x();
    qreal dy = pos.y()-ref.y();
    dx = (dx<0.0) ? (-dx) : dx;
    dy = (dy<0.0) ? (-dy) : dy;

    // +5 is because its hard for human to point exactly into small square
    if(dy<=(cornerSquareSize+5.0) && dx<=(cornerSquareSize+5.0)) return true;

    return false;
}

qreal resizeRect::checkForLimit(qreal previous, qreal next)
{
    // block setting point that will lead to exchange the corners to the other side than their original
    // e.g. topLeft will become topRight, or if y-scaling, topLeft will become bottomLeft, or both cases together -> topLeft will become bottomRight
    qreal boundary = 10.0;
    if(previous<0.0)
        return (next>(-boundary)) ? -boundary : next;
    else
        return (next<boundary) ? boundary : next;

    return next;
}

void resizeRect::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF ePos = event->pos();

    if(isNearAt(ePos, topLeft) || isNearAt(ePos, bottomRight)) QApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
    else if(isNearAt(ePos, topRight) || isNearAt(ePos, bottomLeft)) QApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
    else if(isNearAt(ePos, QPointF(0.0, height/2.0))
             || isNearAt(ePos, QPointF(0.0, -height/2.0))) QApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
    else if(isNearAt(ePos, QPointF(width/2.0, 0.0))
             || isNearAt(ePos, QPointF(-width/2.0, 0.0))) QApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
    else QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));

    QGraphicsItem::hoverMoveEvent(event);
}

void resizeRect::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));

    QGraphicsItem::hoverLeaveEvent(event);
}

void resizeRect::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    if(resize==NONE)
    {
        //prepareGeometryChange();
        QGraphicsItem::mouseMoveEvent(event);
        this->scene()->update();
        return;
    }

    prepareGeometryChange();

    QPointF temp, diffTemp;
    temp = event->pos();

    switch(resize)
    {
        case TOPLEFT:
            diffTemp = (temp-topLeft)/2.0;
            topLeft.setX(checkForLimit(topLeft.x(), topLeft.x()+diffTemp.x()));
            topLeft.setY(checkForLimit(topLeft.y(), topLeft.y()+diffTemp.y()));

            topRight.setX(-topLeft.x());
            topRight.setY(topLeft.y());
            bottomLeft.setX(topLeft.x());
            bottomLeft.setY(-topLeft.y());
            bottomRight.setX(-topLeft.x());
            bottomRight.setY(-topLeft.y());

            break;
        case TOP:
            diffTemp = (temp-topLeft)/2.0;

            topLeft.setY(checkForLimit(topLeft.y(), topLeft.y()+diffTemp.y()));
            topRight.setY(topLeft.y());
            bottomRight.setY(-topLeft.y());
            bottomLeft.setY(-topLeft.y());

            break;
        case TOPRIGHT:
            diffTemp = (temp-topRight)/2.0;

            topRight.setX(checkForLimit(topRight.x(), topRight.x()+diffTemp.x()));
            topRight.setY(checkForLimit(topRight.y(), topRight.y()+diffTemp.y()));
            topLeft.setX(-topRight.x());
            topLeft.setY(topRight.y());
            bottomLeft.setX(-topRight.x());
            bottomLeft.setY(-topRight.y());
            bottomRight.setX(topRight.x());
            bottomRight.setY(-topRight.y());

            break;
        case RIGHT:
            diffTemp = (temp-topRight)/2.0;

            topRight.setX(checkForLimit(topRight.x(), topRight.x()+diffTemp.x()));
            topLeft.setX(-topRight.x());
            bottomRight.setX(topRight.x());
            bottomLeft.setX(-topRight.x());

            break;
        case BOTTOMRIGHT:
            diffTemp = (temp-bottomRight)/2.0;

            bottomRight.setX(checkForLimit(bottomRight.x(), bottomRight.x()+diffTemp.x()));
            bottomRight.setY(checkForLimit(bottomRight.y(), bottomRight.y()+diffTemp.y()));
            topLeft.setX(-bottomRight.x());
            topLeft.setY(-bottomRight.y());
            bottomLeft.setX(-bottomRight.x());
            bottomLeft.setY(bottomRight.y());
            topRight.setX(bottomRight.x());
            topRight.setY(-bottomRight.y());

            break;
        case BOTTOM:
            diffTemp = (temp-bottomRight)/2.0;

            bottomRight.setY(checkForLimit(bottomRight.y(), bottomRight.y()+diffTemp.y()));
            topRight.setY(-bottomRight.y());
            topLeft.setY(-bottomRight.y());
            bottomLeft.setY(bottomRight.y());

            break;
        case BOTTOMLEFT:
            diffTemp = (temp-bottomLeft)/2.0;

            bottomLeft.setX(checkForLimit(bottomLeft.x(), bottomLeft.x()+diffTemp.x()));
            bottomLeft.setY(checkForLimit(bottomLeft.y(), bottomLeft.y()+diffTemp.y()));
            topLeft.setX(bottomLeft.x());
            topLeft.setY(-bottomLeft.y());
            bottomRight.setX(-bottomLeft.x());
            bottomRight.setY(bottomLeft.y());
            topRight.setX(-bottomLeft.x());
            topRight.setY(-bottomLeft.y());

            break;
        case LEFT:
            diffTemp = (temp-bottomLeft)/2.0;

            bottomLeft.setX(checkForLimit(bottomLeft.x(), bottomLeft.x()+diffTemp.x()));
            topLeft.setX(bottomLeft.x());
            bottomRight.setX(-bottomLeft.x());
            topRight.setX(-bottomLeft.x());

            break;
        default: break;
    }

    width = topRight.x()*2.0;
    height = topLeft.y()*2.0;
    // if sides are exchanged
    width = (width<0.0) ? -width : width;
    height = (height<0.0) ? -height : height;

    this->scene()->update();
}

void resizeRect::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // show the selected item on the top of colliding items
    int curr_z_index = this->zValue();
    Q_FOREACH(QGraphicsItem * item, collidingItems())
    {
        if(item->zValue()>=curr_z_index) curr_z_index = item->zValue()+1;
    }
    this->setZValue(curr_z_index);

    QPointF ePos = event->pos();

    if(isNearAt(ePos, topLeft)) resize = TOPLEFT;
    else if(isNearAt(ePos, bottomRight)) resize = BOTTOMRIGHT;
    else if(isNearAt(ePos, topRight)) resize = TOPRIGHT;
    else if(isNearAt(ePos, bottomLeft)) resize = BOTTOMLEFT;
    else if(isNearAt(ePos, QPointF(0.0, height/2.0))) resize = TOP;
    else if(isNearAt(ePos, QPointF(0.0, -height/2.0))) resize = BOTTOM;
    else if(isNearAt(ePos, QPointF(width/2.0, 0.0))) resize = RIGHT;
    else if(isNearAt(ePos, QPointF(-width/2.0, 0.0))) resize = LEFT;
    else resize = NONE;

    QGraphicsItem::mousePressEvent(event);
    this->scene()->update();
}

void resizeRect::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(resize!=NONE)
        resize = NONE;

    QPointF curr_point = mapToScene(0.0, 0.0);
    xPos = curr_point.x();
    yPos = curr_point.y();

    QGraphicsItem::mouseReleaseEvent(event);
    this->scene()->update();
}

