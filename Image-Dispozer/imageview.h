#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWheelEvent>
#include <QGraphicsView>
#include <QMenu>
#include <QAction>
#include <QList>
#include <QGraphicsItem>
#include <QMessageBox>
#include <QMouseEvent>

#include "resizerect.h"
#include "ghostrect.h"
#include "imagescene.h"

class imageView : public QGraphicsView
{
    Q_OBJECT

private:
    double rotationSmoothness;
    bool orthogonalRotation;
    qreal currentScaleFactor;

    QPoint cursorMousePressPosition;

    int vSize, hSize;

    class resizeRect * lastly_selected;

    ghostRect * ghost_rect;

public:
    imageView(imageScene * scene, QWidget * parent = NULL);

    ~imageView();

    void setOrthogonalRotation(bool enable) { orthogonalRotation = enable; }
    bool getOrthogonalRotation(void) { return orthogonalRotation; }
    void setCurrentScaleFactor(qreal scaleFactor) { currentScaleFactor = scaleFactor; }
    qreal getCurrentScaleFactor(void) { return currentScaleFactor; }
    void stretchItem(class resizeRect ** hndl);
    bool checkIfInside(class resizeRect ** hndl);

    void setVSize(int vSizeP) { vSize = vSizeP; }
    void setHSize(int hSizeP) { hSize = hSizeP; }
    int getVSize(void) { return vSize; }
    int getHSize(void) { return hSize; }

protected:
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void showContextMenu(const QPoint & position);
    image_handler * checkForSingleSelection(bool emit_handler_signal = true, bool emit_rect_signal = false, class resizeRect ** selected_rect = NULL);

signals:
    void updateDisplayedItemsVector(class resizeRect * );
    void saveSelectedItemData(class image_handler * );
    void currentSingleItemSelection(class image_handler * );
    void currentSingleItemSelectionRect(class resizeRect * );
    void someItemHasMoved(class image_handler * );
    void someItemHasRotated(class image_handler * );
    void someItemHasChangedDockedState(class image_handler * );
};

#endif // IMAGEVIEW_H
