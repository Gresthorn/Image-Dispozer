#include "imagescene.h"

imageScene::imageScene(QObject *parent)
    : QGraphicsScene(parent)
{
    itemsMode = SINGLE_ITEM;
    resizableItems = false;
    last_z_index = 0;
}

imageScene::~imageScene()
{

}
