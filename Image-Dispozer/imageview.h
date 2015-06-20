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
#include "imagescene.h"

class imageView : public QGraphicsView
{
    Q_OBJECT

private:
    double rotationSmoothness;
    bool orthogonalRotation;
    qreal currentScaleFactor;

public:
    imageView(imageScene * scene, QWidget * parent = NULL);

    ~imageView();

    void setOrthogonalRotation(bool enable) { orthogonalRotation = enable; }
    bool getOrthogonalRotation(void) { return orthogonalRotation; }
    void setCurrentScaleFactor(qreal scaleFactor) { currentScaleFactor = scaleFactor; }
    qreal getCurrentScaleFactor(void) { return currentScaleFactor; }

protected:
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void showContextMenu(const QPoint & position);
    void checkForSingleSelection(void);

signals:
    void updateDisplayedItemsVector(class resizeRect * );
    void saveSelectedItemData(class image_handler * );
    void currentSingleItemSelection(class image_handler * );
};

#endif // IMAGEVIEW_H
