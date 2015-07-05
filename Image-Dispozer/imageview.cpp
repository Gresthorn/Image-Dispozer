#include "imageview.h"

imageView::imageView(imageScene *scene, QWidget * parent)
    : QGraphicsView::QGraphicsView(scene, parent)
{
    rotationSmoothness = 24.0;
    orthogonalRotation = false;
    lastly_selected = NULL;

    // we can now handle our own context menu
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

imageView::~imageView()
{

}


image_handler *imageView::checkForSingleSelection(bool emit_handler_signal, bool emit_rect_signal, resizeRect ** selected_rect)
{
    // check if single item is selected

    image_handler * item = NULL; // default value

    resizeRect * rect_s = NULL;

    if(scene()->selectedItems().count()==1)
    {
        // Retype into resizeRect, since that only we are interested in.
        // If more kind of objects are added to the scene and needs to be
        // considered as well, use dynamic_cast.
        rect_s = qgraphicsitem_cast<resizeRect * >(scene()->selectedItems().first());
        if(rect_s!=NULL) item = rect_s->imageHandlerP(); // obtain image handler pointer

        lastly_selected = rect_s;
    }
    else
    {
        resizeRect * temp_rect=NULL;
        // if lastly_selected is not NULL we will try to find the lastly selected resizeRect
        Q_FOREACH(QGraphicsItem * item, scene()->selectedItems())
        {
            // we will try to find the last selection
            rect_s = qgraphicsitem_cast<resizeRect * >(item);

            // catch the first resizeRect item in case that we coul not found the lastly selected resizeRect
            if(temp_rect==NULL && rect_s!=NULL && rect_s!=0) temp_rect = rect_s;

            if(rect_s!=lastly_selected) rect_s->setSelected(false);
            else temp_rect = rect_s;
        }

        // now set the value either it is lastly selected rect or not
        rect_s = temp_rect;
    }

    // Sometimes this function is called as a common function and the item is required to get as return value.
    // Thus we can achieve this via emit_signal boolean.
    if(emit_handler_signal) emit currentSingleItemSelection(item);

    // If emit_rect_signal is true (by default false) we also emit signal with information about clicked item in scene
    if(emit_rect_signal) emit currentSingleItemSelectionRect(rect_s);

    // if selected_rect is not NULL, somewhere in program we requested the pointer to selected rect as well

    if(selected_rect!=NULL) (*selected_rect) = rect_s;

    return item;
}

void imageView::wheelEvent(QWheelEvent* event) {
    if ((event->modifiers()&Qt::ControlModifier)
                    && event->angleDelta().x() == 0) {

            double by;
            double angle = event->angleDelta().y();

            if      (angle > 0) { by = 1 + ( angle / 360 * 0.1); }
            else if (angle < 0) { by = 1 - (-angle / 360 * 0.1); }
            else                { by = 1; }

            this->scale(by, by);
            // saving scale factor allows better display of borders and corners when zooming
            this->setCurrentScaleFactor(currentScaleFactor/by); // save new scale factor

            event->accept();
    }
    else if(event->modifiers()&Qt::AltModifier)
    {

        // rotate items selected
        if(!scene()->selectedItems().isEmpty())
        {
            // XXX/24.0 is because of smoother rotation (somehow sometimes during ALT pressed, y-delta is zero)
            double angle = event->angleDelta().y()>0 ? event->angleDelta().y()/rotationSmoothness :  event->angleDelta().x()/rotationSmoothness;

            for(int i = 0; i<scene()->selectedItems().count(); i++)
            {
                resizeRect * item = dynamic_cast<resizeRect * >(scene()->selectedItems().at(i));
                // check if pointer is correct
                if(item!=NULL)
                {
                    if(orthogonalRotation) angle = ((angle>0.0) ? 90.0 : -90.0);

                    // calculate if such rotation is possible
                    imageScene * scene = static_cast<imageScene *>(this->scene());
                    QRectF wrapper = item->imageHandlerP()->calculateWrapperCorners(angle);
                    int wrapper_width = (int)(wrapper.width()+0.5);
                    int wrapper_height = (int)(wrapper.height()+0.5);
                    if(wrapper_width>scene->getHSize() || wrapper_height>scene->getVSize())
                    {
                        // this rotation will result in sizes that cannot be accepted in the scene
                        QMessageBox::information((QWidget *)(this->parent()), tr("Unacceptable rotation"), tr("The rotation you wish to set up cannot be accepted since the result object will not be able "
                                                                                       "to be placed in the scene at all."), QMessageBox::Ok);
                        return;
                    }

                    item->incrementRotation(angle);
                    checkIfInside(&item);
                    item->updateExternalData();

                    emit someItemHasRotated(item->imageHandlerP());

                    QGraphicsView::wheelEvent(event);
                }
                else QGraphicsView::wheelEvent(event); // default action if error with pointer occured
            }
        }
    }
    else QGraphicsView::wheelEvent(event);

    // on each interaction we need to check for single
    // item selection (because of updating element info)
    checkForSingleSelection();
}

void imageView::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers().testFlag(Qt::ControlModifier))
    {
        // CTRL is pressed
        if(event->key() == Qt::Key_F)
        {
            // change x flip constant
            for(int i = 0; i<scene()->selectedItems().count(); i++)
            {
                resizeRect * item = dynamic_cast<resizeRect * >(scene()->selectedItems().at(i));
                // check if pointer is correct
                if(item!=NULL)
                {
                    item->switchFlipX();
                    item->update();
                }
                else QGraphicsView::keyPressEvent(event);
            }

        }
        else if(event->key() == Qt::Key_G)
        {
            // change y flip constant
            for(int i = 0; i<scene()->selectedItems().count(); i++)
            {
                resizeRect * item = dynamic_cast<resizeRect * >(scene()->selectedItems().at(i));
                // check if pointer is correct
                if(item!=NULL)
                {
                    item->switchFlipY();
                    item->update();
                }
                else QGraphicsView::keyPressEvent(event);
            }
        }
    }
    else QGraphicsView::keyPressEvent(event);
}

void imageView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);

    // On each interaction we need to check for single
    // item selection (because of updating element info).
    // Also if some single selection is not null, we need to monitor
    // mouse movements. If mouse is pressed and some item
    // is selected, on mouse released we will check
    // if user moved with mouse and emit signal to
    // update group of elements if this is true...

    image_handler * hndl = checkForSingleSelection();

    if(hndl!=NULL && hndl!=0)
    {
        cursorMousePressPosition = event->pos();
    }
}

void imageView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);

    // on each interaction we need to check for single
    // item selection (because of updating element info)

    resizeRect ** rect = new resizeRect *;
    image_handler * hndl = checkForSingleSelection(false, false, rect);

    if(hndl==NULL || hndl==0) return;

    stretchItem(rect);

    checkIfInside(rect);

    // The reason why I have blocked this signal in checkForSingleSelection method
    // and called it manually here is that if the item is at the edge, the emitted
    // signal will cause firstly update values for user in element info group box
    // and after that it will modify the items position, LB corners etc. Now this
    // update is done first and after that the values are displayed for user.
    emit currentSingleItemSelection(hndl);
}

void imageView::stretchItem(resizeRect **hndl)
{
   if((*hndl)==NULL || (*hndl)==0) return;

   QRectF area((*hndl)->getWrapperRect());
   qreal interval = 10.0; // interprets the addition to bounding rect of hndl and increases

   area.setTopLeft(area.topLeft()+QPointF(-interval, interval));
   area.setBottomRight(area.bottomRight()+QPointF(interval, -interval));

   // the area which is used for detecting neighbour objects
   QList<QGraphicsItem * > itemsInScene = this->scene()->items();

   qreal nearestX, nearestY, nearestXabs, nearestYabs;
   bool initializedX = false;
   bool initializedY = false;

   for(int i=0; i<itemsInScene.count(); i++)
   {
       resizeRect * rect = dynamic_cast<resizeRect * >(itemsInScene.at(i));

       if(rect!=NULL && rect!=0)
       {
           // we are not interested in object itself
           if((*hndl)!=rect)
           {
               // right egde of item with left edge of another item
               qreal diff_x = rect->getWrapperRect().topLeft().x()-(*hndl)->getWrapperRect().topRight().x();
               qreal diffAbs_x = diff_x>0.0 ? diff_x : -diff_x;

               // left egde of item with right edge of another item
               qreal diff_x_2 = rect->getWrapperRect().topRight().x()-(*hndl)->getWrapperRect().topLeft().x();
               qreal diffAbs_x_2 = diff_x_2>0.0 ? diff_x_2 : -diff_x_2;

               // decide which of them is closer
               if(diffAbs_x_2<diffAbs_x)
               {
                   diff_x = diff_x_2;
                   diffAbs_x = diffAbs_x_2;
               }

               if(diffAbs_x<interval)
               {
                   if(!initializedX)
                   {
                       nearestX = diff_x;
                       nearestXabs = diffAbs_x;
                       initializedX = true;
                   }
                   else if (diffAbs_x<nearestXabs)
                   {
                       // save only nearest item
                       nearestX = diff_x;
                       nearestXabs = diffAbs_x;
                   }
               }


               // top egde of item with bottom edge of another item
               qreal diff_y = rect->getWrapperRect().bottomLeft().y()-(*hndl)->getWrapperRect().topRight().y();
               qreal diffAbs_y = diff_y>0.0 ? diff_y : -diff_y;

               // bottom egde of item with top edge of another item
               qreal diff_y_2 = rect->getWrapperRect().topRight().y()-(*hndl)->getWrapperRect().bottomLeft().y();
               qreal diffAbs_y_2 = diff_y_2>0.0 ? diff_y_2 : -diff_y_2;

               // decide which of them is closer
               if(diffAbs_y_2<diffAbs_y)
               {
                   diff_y = diff_y_2;
                   diffAbs_y = diffAbs_y_2;
               }

               if(diffAbs_y<interval)
               {
                   if(!initializedY)
                   {
                       nearestY = diff_y;
                       nearestYabs = diffAbs_y;
                       initializedY = true;
                   }
                   else if (diffAbs_y<nearestYabs)
                   {
                       // save only nearest item
                       nearestY = diff_y;
                       nearestYabs = diffAbs_y;
                   }
               }
           }
       }
   }

   QPointF new_pos((*hndl)->pos().x(), (*hndl)->pos().y());
   // if some item was found, reposition item
   if(initializedX) new_pos.setX(new_pos.x()+nearestX);
   if(initializedY) new_pos.setY(new_pos.y()+nearestY);

   if(initializedX || initializedY) (*hndl)->setPos(new_pos);
}

bool imageView::checkIfInside(resizeRect **hndl)
{
    bool parameter_changed = false; // is set to true if we modified something

    if((*hndl)!=NULL && (*hndl)!=0)
    {
        QRectF wrapperRect = (*hndl)->getWrapperRect();
        qreal wrapperWidth = wrapperRect.width()>0.0 ? wrapperRect.width() : -wrapperRect.width();
        qreal wrapperHeight = wrapperRect.height()>0.0 ? wrapperRect.height() : -wrapperRect.height();

        // check if item is not leaving right or left border
        if(wrapperRect.topRight().x()>hSize)
        {
            parameter_changed = true;
            (*hndl)->setX(hSize-wrapperWidth/2.0);
        }
        else if(wrapperRect.bottomLeft().x()<0.0)
        {
            parameter_changed = true;
            (*hndl)->setX(wrapperWidth/2.0);
        }

        // check if item is not leaving top or bottom border
        if(wrapperRect.topLeft().y()>vSize)
        {
            parameter_changed = true;
            (*hndl)->setY(vSize-wrapperHeight/2.0);
        }
        else if(wrapperRect.bottomLeft().y()<0.0)
        {
            parameter_changed = true;
            (*hndl)->setY(wrapperHeight/2.0);
        }

        (*hndl)->updateCurrentPosition();
    }

    return parameter_changed;
}

void imageView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);

    // On each interaction we need to check for single
    // item selection (because of updating element info).
    // Also if some item is selected, then we need to check
    // if cursor was moved since it was pressed

    image_handler * hndl = checkForSingleSelection();

    if(hndl!=NULL && hndl!=0)
    {
        if(cursorMousePressPosition!=event->pos())
        {
            emit someItemHasMoved(hndl);
        }
    }
}

void imageView::showContextMenu(const QPoint &position)
{
    // build new context menu and get its global position at cursor
    QPoint globalPos = mapToGlobal(position);
    QMenu itemContextMenu;

    // create list with items
    QList<QAction * > actionsList;
    QAction * hideSelected = new QAction(tr("Hide selected"), &itemContextMenu);
    actionsList.append(hideSelected);
    QAction * saveData = new QAction(tr("Save items data"), &itemContextMenu);
    actionsList.append(saveData);
    QAction * undockItem = new QAction(tr("Docked"), &itemContextMenu);
    undockItem->setCheckable(true);
    actionsList.append(undockItem);

    // set check state according to settings of item (undocked/docked)
    image_handler * hndlr = NULL;
    resizeRect * item = NULL;
    if(scene()->selectedItems().count()==1)
    {
        item = qgraphicsitem_cast<resizeRect * >(scene()->selectedItems().first());
        if(item!=NULL && item!=0)
        {
            hndlr = item->imageHandlerP();
            if(hndlr->isDocked()) undockItem->setChecked(true);
            else undockItem->setChecked(false);
        }
    }
    else
    {
        // disable saveData and undockItem options if 0 or more than 1 items are selected
        saveData->setDisabled(true);
        undockItem->setDisabled(true);
    }

    // fill list with desired actions
    itemContextMenu.addActions(actionsList);

    QAction * selected = itemContextMenu.exec(globalPos);

    // if some item was selected
    if(selected)
    {
        if(selected==hideSelected)
        {
            // now hide selected items
            for(int i=0; i<scene()->selectedItems().count(); i++)
            {

                // Retype into resizeRect, since that only we are interested in.
                // If more kind of objects are added to the scene and needs to be
                // considered as well, use dynamic_cast.
                item = qgraphicsitem_cast<resizeRect * >(scene()->selectedItems().at(i));
                if(item!=NULL) item->deleteLater();

                emit updateDisplayedItemsVector(item);
            }
        }
        else if(selected==saveData)
        {
            // We already casted the single item before. If some was found, item should contain the pointer to it.
            // If more items or no item was selected, this option will be not enabled so we even do not need to
            // check if pointer is NULL.

            // item = qgraphicsitem_cast<resizeRect * >(scene()->selectedItems().first());

            // emit signal with pointer to image handler containing data
            emit saveSelectedItemData(item->imageHandlerP());
        }
        else if(selected==undockItem)
        {
            // If more items or no item was selected, this option will be not enabled so we even do not need to
            // check if pointer is NULL.
            hndlr->setDocked(!hndlr->isDocked());

            // inform main window about this change so it can change the icon for that item
            emit someItemHasChangedDockedState(hndlr);
        }
    }

    // delete all actions
    while(!actionsList.empty())
    {
        delete actionsList.first();
        actionsList.removeFirst();
    }
}

