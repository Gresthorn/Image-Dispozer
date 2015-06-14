#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWheelEvent>
#include <QGraphicsView>
#include <QMenu>
#include <QAction>
#include <QList>
#include <QGraphicsItem>

#include "imagescene.h"
#include "resizerect.h"

class imageView : public QGraphicsView
{
    Q_OBJECT

private:
    double rotationSmoothness;
    bool orthogonalRotation;

public:
    imageView(imageScene *scene, QWidget *parent=NULL);
    ~imageView();

    void setOrthogonalRotation(bool enable) { orthogonalRotation = enable; }
    bool getOrthogonalRotation(void) { return orthogonalRotation; }

protected:
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

public slots:
    void showContextMenu(const QPoint & position);
};

#endif // IMAGEVIEW_H
