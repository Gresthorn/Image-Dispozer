#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QWheelEvent>
#include <QGraphicsView>

#include "imagescene.h"
#include "resizerect.h"

class imageView : public QGraphicsView
{
    Q_OBJECT

public:
    imageView(imageScene *scene, QWidget *parent=NULL);
    ~imageView();

private:
    double rotationSmoothness;

protected:
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
};

#endif // IMAGEVIEW_H
