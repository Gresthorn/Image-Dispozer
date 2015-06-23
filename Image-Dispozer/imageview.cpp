#include "imageview.h"

imageView::imageView(imageScene *scene, QWidget * parent)
    : QGraphicsView::QGraphicsView(scene, parent)
{
    rotationSmoothness = 24.0;
    orthogonalRotation = false;

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

    resizeRect * rect_s;

    if(scene()->selectedItems().count()==1)
    {
        // Retype into resizeRect, since that only we are interested in.
        // If more kind of objects are added to the scene and needs to be
        // considered as well, use dynamic_cast.
        rect_s = qgraphicsitem_cast<resizeRect * >(scene()->selectedItems().first());
        if(rect_s!=NULL) item = rect_s->imageHandlerP(); // obtain image handler pointer
    }

    // Sometimes this function is called as a common function and the item is required to get as return value.
    // Thus we can achieve this via emit_signal boolean.
    if(emit_handler_signal) emit currentSingleItemSelection(item);

    // If emit_rect_signal is true (by default false) we also emit signal with information about clicked item in scene
    if(emit_rect_signal) emit currentSingleItemSelectionRect(rect_s);

    // if selected_rect is not NULL, somewhere in program we requested the pointer to selected rect as well
    if(selected_rect!=NULL) *selected_rect = rect_s;

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
                    item->incrementRotation(angle);
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

    // on each interaction we need to check for single
    // item selection (because of updating element info)
    checkForSingleSelection();
}

void imageView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);

    // on each interaction we need to check for single
    // item selection (because of updating element info)
    checkForSingleSelection();
}

void imageView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);

    // on each interaction we need to check for single
    // item selection (because of updating element info)
    checkForSingleSelection();
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

    // disable saveData option if 0 or more than 1 items are selected
    if(scene()->selectedItems().count()!=1) saveData->setDisabled(true);

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
                resizeRect * item = qgraphicsitem_cast<resizeRect * >(scene()->selectedItems().at(i));
                if(item!=NULL) item->deleteLater();

                emit updateDisplayedItemsVector(item);
            }
        }
        else if(selected==saveData)
        {
            resizeRect * item = qgraphicsitem_cast<resizeRect * >(scene()->selectedItems().first());
            // emit signal with pointer to image handler containing data
            emit saveSelectedItemData(item->imageHandlerP());
        }
    }

    // delete all actions
    while(!actionsList.empty())
    {
        delete actionsList.first();
        actionsList.removeFirst();
    }
}

