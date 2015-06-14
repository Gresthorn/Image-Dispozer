#ifndef IMAGESCENE_H
#define IMAGESCENE_H

#include <QGraphicsScene>

class imageScene : public QGraphicsScene
{
    Q_OBJECT

public:
    imageScene(QObject *parent = NULL);
    ~imageScene();
};

#endif // IMAGESCENE_H
