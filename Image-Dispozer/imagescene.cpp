#include "imagescene.h"

imageScene::imageScene(QObject *parent)
    : QGraphicsScene(parent)
{
    itemsMode = SINGLE_ITEM;
}

imageScene::~imageScene()
{

}

