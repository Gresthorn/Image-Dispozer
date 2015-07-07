#ifndef IMAGESCENE_H
#define IMAGESCENE_H

#include <QGraphicsScene>
#include <QDebug>
#include <QGraphicsItem>

enum sceneItemsMode
{
    SINGLE_ITEM,
    MULTIPLE_ITEMS
};

class imageScene : public QGraphicsScene
{
    Q_OBJECT

private:
    sceneItemsMode itemsMode;
    bool resizableItems;

    int vSize;
    int hSize;

    unsigned long int last_z_index;

public:
    imageScene(QObject *parent = NULL);
    ~imageScene();

    sceneItemsMode getSceneItemsMode(void) { return itemsMode; }
    void setSceneItemsMode(sceneItemsMode mode) { itemsMode = mode; }
    void setResizableItems(bool enable) { resizableItems = enable; }
    bool getResizableItems(void) { return resizableItems; }
    void setVSize(int vSizeP) { vSize = vSizeP; }
    void setHSize(int hSizeP) { hSize = hSizeP; }
    int getVSize(void) { return vSize; }
    int getHSize(void) { return hSize; }
    unsigned long int getZIndex(void) { return last_z_index; }
    void setZIndex(unsigned long int zindex) { last_z_index = zindex; }
    void incrementZIndex(void) { ++last_z_index; }
    void decrementZIndex(void) { (last_z_index>0) ? --last_z_index : last_z_index = 0; }
};

#endif // IMAGESCENE_H
