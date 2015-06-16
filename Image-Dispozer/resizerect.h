#ifndef RESIZERECT_H
#define RESIZERECT_H

#include <QGraphicsItem>
#include <QRectF>
#include <QPointF>
#include <QPainter>
#include <QObject>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>
#include <QApplication>
#include <QCursor>
#include <QPixmap>
#include <QDebug>

#include "image_handler.h"
#include "imageview.h"

//! This class allows to create a resizable graphical item with border and markers for resize actions.
/*!
 * This class was created because of need to draw a pixmap with ability to be
 * resized by mouse drag. It provides a markers which allows user to click and
 * resize in desired direction. Class also contains a protection against too
 * small resizing or exchanging the corners positions (e.g. top left corner becomes
 * bottom right if user drags it in that direction).
 *
 * Variables resizeRect#constantZIndex provides options to set one z-index for the
 * item. The z-index will be changed to the toppest only in case of selecting the item. After leaving it, z-index is returned back.
 * If the constant indexing is disabled in the scene, new z-index will be set after
 * each selection of the item and remains on the top.
 *
 * Bounding rectangle is explicitly and for comfort usage defined by all four corners
 * at resizeRect#topLeft , resizeRect#topRight , resizeRect#bottomLeft and resizeRect#bottomRight.
 * Position, width and height can be accessed from the outside via public methods
 * resizeRect::getCurrentPosition and resizeRect::getCurrentSize. Or you can use standard
 * Qt methods, since it inherits QGraphicsItem.
 *
 */

class resizeRect :  public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

private:

    //! Enumeration type for identifying resize method currently in use.
    /*!
     * If the user clicks the item, detection functions will try to get
     * information about wether some of the markers was clicked. If yes
     * appropriate resize method is specified and while dragging, program
     * can select the algorithms for given resize method.
     *
     * \sa resizeRect#resize
     */
    enum resizeType
    {
        TOPLEFT, /*!< Top left corner marker was clicked. */
        TOP, /*!< Top edge marker was clicked. */
        TOPRIGHT, /*!< Top right corner marker was clicked. */
        RIGHT, /*!< Top right edge marker was clicked. */
        BOTTOMRIGHT, /*!< Bottom right marker was clicked. */
        BOTTOM, /*!< Bottom edge marker was clicked. */
        BOTTOMLEFT, /*!< Bottom left corner marker was clicked. */
        LEFT, /*!< Left edge marker was clicked. */
        NONE /*!< Item was clicked at position where no marker is present. */
    };

    //! Variable which stores the currently used resize method
    /*!
     * If the user clicks the item, inner function will detect, where the
     * click event occured and assign appropriate resize method type, which
     * is stored here. While dragging, this variable is being checked for
     * next method selections.
     *
     * \sa resizeRect::resizeType
     */
    resizeType resize;


    int constantZIndex;

    image_handler * image;
    qreal flipX, flipY;

    qreal xPos, yPos, width, height;

    const qreal cornerSquareSize;

    QPointF topLeft, topRight, bottomLeft, bottomRight;

    bool isNearAt(QPointF pos, QPointF ref);
    qreal checkForLimit(qreal previous, qreal next);
    QPointF getWrapperBottomLeft(void);

    enum { RESIZE_RECT_ITEM = QVariant::UserType+100 };
    int type() const { return RESIZE_RECT_ITEM; }

protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

public:
    resizeRect(qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent);
    ~resizeRect();

    void incrementRotation(qreal angle);
    void switchFlipX(void);
    void switchFlipY(void);
    QPointF getCurrentPosition(void) { return QPointF(xPos, yPos); }
    QSize getCurrentSize(void) { return QSize(width, height); }

    void setPixmap(image_handler *handler);
    void prepareForDeletion(void);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;

signals:
    void isBeingDeleted(resizeRect * );
};

#endif // RESIZERECT_H
