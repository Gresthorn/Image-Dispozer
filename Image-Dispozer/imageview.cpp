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
    else if(event->modifiers()==Qt::AltModifier)
    {
        // rotate items selected
        if(!scene()->selectedItems().isEmpty())
        {
            double angle = event->angleDelta().y()/rotationSmoothness; // XXX/24.0 is because of smoother rotation
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

void imageView::showContextMenu(const QPoint &position)
{
    // build new context menu and get its global position at cursor
    QPoint globalPos = mapToGlobal(position);
    QMenu itemContextMenu;

    // create list with items
    QList<QAction * > actionsList;
    QAction * hideSelected = new QAction(tr("Hide selected"), &itemContextMenu);
    actionsList.append(hideSelected);

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
    }

    // delete all actions
    while(!actionsList.empty())
    {
        delete actionsList.first();
        actionsList.removeFirst();
    }
}

