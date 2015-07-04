#include "resizerect.h"

resizeRect::resizeRect(qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent)
    : QGraphicsItem(parent), cornerSquareSize(5.0), PI(3.141592653589793238462643383279502884197169399375105820974944592307816406286)
{
    xPos = x;
    yPos = y;
    width = w;
    height = h;
    itemRotation = 0.0;

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

    image = NULL;

    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable);

    setPos(x, y);
    setAcceptHoverEvents(true);
    setAcceptDrops(true);

    calculateWrapperCorners();
}

resizeRect::~resizeRect()
{
    // DO NOT DELETE 'image' SINCE THE HANDLER IS USED ALSO OUTSIDE THIS OBJECT !!!!
    image->setCurrentlyDisplayed(false);
}

void resizeRect::incrementRotation(qreal angle)
{
    qreal temp_rot = itemRotation+angle;
    temp_rot = (((temp_rot)<0.0) ? temp_rot+360.0 : temp_rot);

    if(qFuzzyCompare(temp_rot, 360))
    {
        //setRotation(0.0);
        itemRotation = 0.0;
        calculateWrapperCorners();

        // NOTE THAT THE FOLLOWING CODE IS NOT NEEDED ANY MORE SINCE IT TURNS THAT IT IS
        // BETTER TO MAKE GLOBAL UPDATE OF EXTERNAL DATA IN GRAPHICS VIEW
        // also update wrapper's bottom left corner which changes with rotating as well
        //image->setLBCorner(getWrapperBottomLeft());
        //image->setItemRotation(0.0);
    }
    else
    {
        if(temp_rot>360.0) temp_rot = 90.0;
        //setRotation(temp_rot);
        itemRotation = temp_rot;
        calculateWrapperCorners();

        // NOTE THAT THE FOLLOWING CODE IS NOT NEEDED ANY MORE SINCE IT TURNS THAT IT IS
        // BETTER TO MAKE GLOBAL UPDATE OF EXTERNAL DATA IN GRAPHICS VIEW
        // also update wrapper's bottom left corner which changes with rotating as well
        //image->setLBCorner(getWrapperBottomLeft());
        //image->setItemRotation(temp_rot);
    }

    this->scene()->update();
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
    image = handler;
}

void resizeRect::updateData()
{
    QSizeF sz = image->getItemSize();
    QPointF pt = image->getPosition();
    qreal rot = image->getItemRotation();

    // firstly set position
    this->setPos(pt);
    xPos = pt.x();
    yPos = pt.y();

    // now update width and height
    width = sz.width();
    height = sz.height();

    // now update corners coordinates
    topLeft.setX(-width/2.0); topLeft.setY(height/2.0);
    topRight.setX(width/2.0); topRight.setY(height/2.0);
    bottomRight.setX(width/2.0); bottomRight.setY(-height/2.0);
    bottomLeft.setX(-width/2.0); bottomLeft.setY(-height/2.0);

    // finally we will rotate item
    setItemRotation(rot);

    calculateWrapperCorners();

    // Sometimes if some element in group is modified, the group is updated automatically.
    // However those modifications can be not appropriate for "this" item and we need to
    // check them.
    imageView * view = static_cast<imageView * >(this->scene()->views().first());
    resizeRect * this_item = this;
    if(view->checkIfInside(&this_item))
    {
        // after check if item is inside, its position is updated, we need to modify handler as well
        this->updateExternalData();
    }

    // update scene so changes can directly be visible
    this->scene()->update();
}

void resizeRect::updateExternalData()
{
    image->setItemRotation(this->getItemRotation());
    image->setItemSize(this->getCurrentSize());
    image->setPosition(this->getCurrentPosition());
    image->setLBCorner(this->getWrapperBottomLeft());
}

void resizeRect::calculateWrapperCorners()
{
    // calculate diagonal line length between [0, 0] and corner
    qreal diag_length = sqrt(pow(width/2.0, 2.0)+pow(height/2.0, 2.0));
    qreal offset_angle_right = asin((height/2.0)/diag_length);
    qreal offset_angle_left = PI-offset_angle_right;

    qreal r_angle_radians = (itemRotation/180.0)*PI;

    // change angles in 2nd and 4th quadrant
    if((r_angle_radians>=PI/2.0 && r_angle_radians<PI) ||
            ((r_angle_radians>=((PI/2.0)*3.0)) && r_angle_radians<(2.0*PI))) r_angle_radians = (PI/2.0-r_angle_radians)+PI/2.0;

    qreal calc_angle_right = offset_angle_right+r_angle_radians;
    qreal calc_angle_left = offset_angle_left+r_angle_radians;

    //qreal top_right_x = cos(calc_angle_right)*diag_length;
    //qreal top_left_y = sin(calc_angle_left)*diag_length

    qreal top_right_y = sin(calc_angle_right)*diag_length;
    qreal top_left_x = cos(calc_angle_left)*diag_length;

    // make correction for angles greater or equal to 180.0 otherwise rect will be mirrored and flipped
    if(itemRotation>=180.0)
    {
        top_right_y = -top_right_y;
        top_left_x = -top_left_x;
    }

    // calculate reference corners
    topLeftW.setX(top_left_x);
    topLeftW.setY(top_right_y);

    bottomRightW.setX(-top_left_x);
    bottomRightW.setY(-top_right_y);

    // calculate other corners
    topRightW.setX(-top_left_x);
    topRightW.setY(top_right_y);

    bottomLeftW.setX(top_left_x);
    bottomLeftW.setY(-top_right_y);
}

QRectF resizeRect::getWrapperRect()
{
    QRectF r(mapToScene(topLeftW), mapToScene(bottomRightW));
    return r;

}

void resizeRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if(isSelected())
    {

        painter->save();
        painter->drawRect(QRectF(topLeftW, bottomRightW));
        painter->restore();

        // if item is selected, draw also bordering with resize handlers
        painter->save();

        imageView * view = static_cast<imageView * >(scene()->views().first());
        qreal curr_scale = view->getCurrentScaleFactor();

        qreal scaledCorner = cornerSquareSize*curr_scale*0.5; // * 0.5 is because for nicer display

        //painter->setPen(QPen(QBrush(Qt::darkRed), 0.8*curr_scale, Qt::SolidLine));
        //painter->drawRect(QRectF(topLeft, bottomRight));

        // draw small squares to the corners and in the middle of borders
        qreal cssHalf = scaledCorner/2.0;

        painter->rotate(itemRotation);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(QColor(Qt::darkGreen)));

        painter->drawRect(QRectF(topLeft.x()-cssHalf, topLeft.y()-cssHalf, scaledCorner, scaledCorner)); // topLeft square
        painter->drawRect(QRectF(topRight.x()-cssHalf, topRight.y()-cssHalf, scaledCorner, scaledCorner)); // topRight square
        painter->drawRect(QRectF(bottomLeft.x()-cssHalf, bottomLeft.y()-cssHalf, scaledCorner, scaledCorner)); // bottomLeft square
        painter->drawRect(QRectF(bottomRight.x()-cssHalf, bottomRight.y()-cssHalf, scaledCorner, scaledCorner)); // bottomLeft square

        /*painter->drawRect(QRectF(-cssHalf, height/2.0-cssHalf, scaledCorner, scaledCorner)); // top border
        painter->drawRect(QRectF(width/2.0-cssHalf, -cssHalf, scaledCorner, scaledCorner)); // right border
        painter->drawRect(QRectF(-cssHalf, -height/2.0-cssHalf, scaledCorner, scaledCorner)); // bottom border
        painter->drawRect(QRectF(-width/2.0-cssHalf, -cssHalf, scaledCorner, scaledCorner)); // left border*/

        painter->restore();
    }

    painter->save();
    painter->scale(flipX*1.0, flipY*1.0);
    painter->rotate(-itemRotation);
    if(image!=NULL)
    {
        if(image->isFileCorrect())
            painter->drawPixmap(topLeft.x(), -topLeft.y(), width, height, *image);
        else
        {
            // if missing pixmap, draw rectangle with cross inside
            painter->drawRect(QRectF(topLeft, bottomRight));
            painter->drawLine(topLeft, bottomRight);
            painter->drawLine(topRight, bottomLeft);
        }
    }
    else
    {
        // if missing pixmap, draw rectangle with cross inside
        painter->drawRect(QRectF(topLeft, bottomRight));
        painter->drawLine(topLeft, bottomRight);
        painter->drawLine(topRight, bottomLeft);
    }
    painter->restore();
}

QRectF resizeRect::boundingRect() const
{
    // subtrackt full cornerSquareSize (not only half) will ensure better repainting while changing size of widget
    //QRectF r((topLeft.x()-cornerSquareSize), -(topLeft.y()+cornerSquareSize), width+cornerSquareSize*2.0, height+cornerSquareSize*2.0);

    QPointF t_l = topLeftW;
    t_l.setY(-t_l.y());

    QPointF b_r = bottomRightW;
    b_r.setY(-b_r.y());

    QRectF r(t_l, b_r);

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

qreal resizeRect::checkForLimit(qreal previous, qreal next, bool * boundary)
{
    // block setting point that will lead to exchange the corners to the other side than their original
    // e.g. topLeft will become topRight, or if y-scaling, topLeft will become bottomLeft, or both cases together -> topLeft will become bottomRight
    qreal boundary_val = 10.0;
    *boundary = false;
    if(previous<0.0)
    {
        if(next>(-boundary_val))
        {
            *boundary = true;
            return -boundary_val;
        }
        else
            return next;
    }
    else
    {
        if(next<boundary_val)
        {
            *boundary = true;
            return boundary_val;
        }
        else
            return next;
    }

    return next;
}

QPointF resizeRect::getWrapperBottomLeft()
{
    // Wrapper is rectangle connecting all corners and does not matter on rotation of item.
    /*QPointF x, y; // points holding x and y position
    qreal rotAngle = rotation();
    if(rotAngle>=0.0 && rotAngle<90.0)
    {
        x = mapToScene(topLeft);
        y = mapToScene(bottomLeft);

        return QPointF(x.x(), y.y());
    }
    else if(rotAngle>=90.0 && rotAngle<180.0)
    {
        x = mapToScene(topRight);
        y = mapToScene(topLeft);
        return QPointF(x.x(), y.y());
    }
    else if(rotAngle>=180.0 && rotAngle<270.0)
    {
        x = mapToScene(bottomRight);
        y = mapToScene(topRight);
        return QPointF(x.x(), y.y());
    }
    else
    {
        x = mapToScene(bottomLeft);
        y = mapToScene(bottomRight);
        return QPointF(x.x(), y.y());
    }

    return QPointF(xPos, yPos); // returns at least central point*/

    return mapToScene(bottomLeftW);
}

void resizeRect::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF ePos = event->pos();

    imageScene * scene = static_cast<imageScene * >(this->scene());

    if(scene->getResizableItems())
    {
        if(isNearAt(ePos, topLeft) || isNearAt(ePos, bottomRight)) QApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
        else if(isNearAt(ePos, topRight) || isNearAt(ePos, bottomLeft)) QApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
        else if(isNearAt(ePos, QPointF(0.0, height/2.0))
                 || isNearAt(ePos, QPointF(0.0, -height/2.0))) QApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
        else if(isNearAt(ePos, QPointF(width/2.0, 0.0))
                 || isNearAt(ePos, QPointF(-width/2.0, 0.0))) QApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
        else QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
    }
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

        // update data in image, if real time update is enabled
        QPointF curr_point = mapToScene(0.0, 0.0);
        xPos = curr_point.x();
        yPos = curr_point.y();

        this->calculateWrapperCorners();

        image->setPosition(curr_point);
        image->setItemSize(QSizeF(width, height));
        image->setLBCorner(getWrapperBottomLeft());

        return;
    }

    prepareGeometryChange();

    QPointF temp, diffTemp, pt;
    temp = event->pos();
    bool boundary_x, boundary_y;

    switch(resize)
    {
        case TOPLEFT:
            diffTemp = (temp-topLeft)/2.0;
            topLeft.setX(checkForLimit(topLeft.x(), topLeft.x()+diffTemp.x(), &boundary_x));
            topLeft.setY(checkForLimit(topLeft.y(), topLeft.y()+diffTemp.y(), &boundary_y));

            topRight.setX(-topLeft.x());
            topRight.setY(topLeft.y());
            bottomLeft.setX(topLeft.x());
            bottomLeft.setY(-topLeft.y());
            bottomRight.setX(-topLeft.x());
            bottomRight.setY(-topLeft.y());

            pt = mapToScene(diffTemp);

            if(!boundary_x) this->setX(pt.x());
            if(!boundary_y) this->setY(pt.y());

            break;
        case TOP:
            diffTemp = (temp-topLeft)/2.0;

            topLeft.setY(checkForLimit(topLeft.y(), topLeft.y()+diffTemp.y(), &boundary_y));
            topRight.setY(topLeft.y());
            bottomRight.setY(-topLeft.y());
            bottomLeft.setY(-topLeft.y());  

            pt = mapToScene(diffTemp);

            if(!boundary_y) this->setY(pt.y());

            break;
        case TOPRIGHT:
            diffTemp = (temp-topRight)/2.0;

            topRight.setX(checkForLimit(topRight.x(), topRight.x()+diffTemp.x(), &boundary_x));
            topRight.setY(checkForLimit(topRight.y(), topRight.y()+diffTemp.y(), &boundary_y));
            topLeft.setX(-topRight.x());
            topLeft.setY(topRight.y());
            bottomLeft.setX(-topRight.x());
            bottomLeft.setY(-topRight.y());
            bottomRight.setX(topRight.x());
            bottomRight.setY(-topRight.y());

            pt = mapToScene(diffTemp);

            if(!boundary_x) this->setX(pt.x());
            if(!boundary_y) this->setY(pt.y());

            break;
        case RIGHT:
            diffTemp = (temp-topRight)/2.0;

            topRight.setX(checkForLimit(topRight.x(), topRight.x()+diffTemp.x(), &boundary_x));
            topLeft.setX(-topRight.x());
            bottomRight.setX(topRight.x());
            bottomLeft.setX(-topRight.x());

            pt = mapToScene(diffTemp);

            if(!boundary_x) this->setX(pt.x());

            break;
        case BOTTOMRIGHT:
            diffTemp = (temp-bottomRight)/2.0;

            bottomRight.setX(checkForLimit(bottomRight.x(), bottomRight.x()+diffTemp.x(), &boundary_x));
            bottomRight.setY(checkForLimit(bottomRight.y(), bottomRight.y()+diffTemp.y(), &boundary_y));
            topLeft.setX(-bottomRight.x());
            topLeft.setY(-bottomRight.y());
            bottomLeft.setX(-bottomRight.x());
            bottomLeft.setY(bottomRight.y());
            topRight.setX(bottomRight.x());
            topRight.setY(-bottomRight.y());

            pt = mapToScene(diffTemp);

            if(!boundary_x) this->setX(pt.x());
            if(!boundary_y) this->setY(pt.y());

            break;
        case BOTTOM:
            diffTemp = (temp-bottomRight)/2.0;

            bottomRight.setY(checkForLimit(bottomRight.y(), bottomRight.y()+diffTemp.y(), &boundary_y));
            topRight.setY(-bottomRight.y());
            topLeft.setY(-bottomRight.y());
            bottomLeft.setY(bottomRight.y());

            pt = mapToScene(diffTemp);

            if(!boundary_y) this->setY(pt.y());

            break;
        case BOTTOMLEFT:
            diffTemp = (temp-bottomLeft)/2.0;

            bottomLeft.setX(checkForLimit(bottomLeft.x(), bottomLeft.x()+diffTemp.x(), &boundary_x));
            bottomLeft.setY(checkForLimit(bottomLeft.y(), bottomLeft.y()+diffTemp.y(), &boundary_y));
            topLeft.setX(bottomLeft.x());
            topLeft.setY(-bottomLeft.y());
            bottomRight.setX(-bottomLeft.x());
            bottomRight.setY(bottomLeft.y());
            topRight.setX(-bottomLeft.x());
            topRight.setY(-bottomLeft.y());

            pt = mapToScene(diffTemp);

            if(!boundary_x) this->setX(pt.x());
            if(!boundary_y) this->setY(pt.y());

            break;
        case LEFT:
            diffTemp = (temp-bottomLeft)/2.0;

            bottomLeft.setX(checkForLimit(bottomLeft.x(), bottomLeft.x()+diffTemp.x(), &boundary_x));
            topLeft.setX(bottomLeft.x());
            bottomRight.setX(-bottomLeft.x());
            topRight.setX(-bottomLeft.x());

            pt = mapToScene(diffTemp);

            if(!boundary_x) this->setX(pt.x());

            break;
        default: break;
    }

    width = topRight.x()*2.0;
    height = topLeft.y()*2.0;
    // if sides are exchanged
    width = (width<0.0) ? -width : width;
    height = (height<0.0) ? -height : height;

    QPointF curr_point = mapToScene(0.0, 0.0);
    xPos = curr_point.x();
    yPos = curr_point.y();


    this->calculateWrapperCorners();

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

    imageScene * scene = static_cast<imageScene * >(this->scene());

    if(scene->getResizableItems())
    {
        if(isNearAt(ePos, topLeft)) resize = TOPLEFT;
        else if(isNearAt(ePos, bottomRight)) resize = BOTTOMRIGHT;
        else if(isNearAt(ePos, topRight)) resize = TOPRIGHT;
        else if(isNearAt(ePos, bottomLeft)) resize = BOTTOMLEFT;
        else if(isNearAt(ePos, QPointF(0.0, height/2.0))) resize = TOP;
        else if(isNearAt(ePos, QPointF(0.0, -height/2.0))) resize = BOTTOM;
        else if(isNearAt(ePos, QPointF(width/2.0, 0.0))) resize = RIGHT;
        else if(isNearAt(ePos, QPointF(-width/2.0, 0.0))) resize = LEFT;
        else resize = NONE;
    }
    else resize = NONE;

    this->calculateWrapperCorners();

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

    // save new data to handler, so they can be loaded later if graphics item is deleted and revealed
    image->setPosition(QPointF(xPos, yPos));
    image->setItemSize(QSizeF(width, height));
    image->setLBCorner(getWrapperBottomLeft());

    this->calculateWrapperCorners();

    QGraphicsItem::mouseReleaseEvent(event);
    this->scene()->update();
}

