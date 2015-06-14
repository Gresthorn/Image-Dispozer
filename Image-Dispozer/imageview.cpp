#include "imageview.h"

imageView::imageView(imageScene *scene, QWidget *parent)
    : QGraphicsView::QGraphicsView(scene, parent)
{
    rotationSmoothness = 24.0;
}

imageView::~imageView()
{

}

void imageView::wheelEvent(QWheelEvent* event) {
    if ((event->modifiers()&Qt::ControlModifier) == Qt::ControlModifier
                    && event->angleDelta().x() == 0) {

            double by;
            double angle = event->angleDelta().y();

            if      (angle > 0) { by = 1 + ( angle / 360 * 0.1); }
            else if (angle < 0) { by = 1 - (-angle / 360 * 0.1); }
            else                { by = 1; }

            this->scale(by, by);

            event->accept();
    }
    else if(event->modifiers()==Qt::AltModifier)
    {
        // rotate items selected
        if(!scene()->selectedItems().isEmpty())
        {
            double angle = event->angleDelta().x()/rotationSmoothness; // XXX/24.0 is because of smoother rotation
            for(int i = 0; i<scene()->selectedItems().count(); i++)
            {
                resizeRect * item = dynamic_cast<resizeRect * >(scene()->selectedItems().at(i));
                // check if pointer is correct
                if(item!=NULL)
                {
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

