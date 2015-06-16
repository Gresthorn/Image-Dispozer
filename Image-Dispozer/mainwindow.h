#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QStringList>
#include <QList>
#include <QFile>
#include <QDebug>
#include <QGraphicsRectItem>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QSet>

#include "imageselector.h"
#include "image_handler.h"
#include "rolestring.h"
#include "imageview.h"
#include "imagescene.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QList<image_handler * > * imageItems;
    QList<resizeRect * > * displayedItems;
    QStringList * importedImages;
    QVector<roleString * > * rolesList;

    imageScene * scene;
    imageView * view;
    qreal hSize;
    qreal vSize;

    QGraphicsRectItem * borderRect;

    void initializeTreeItems(void);
    void updateRolesListWidget(void);
    void updateRolesListWidgetColor(void);

public slots:
    void removeAllDisplayedItems(void);
    void imageSelectorWindow(void);
    void displayNewRectItem(int row);
    void updateDisplayedItemsVector(resizeRect * item);
};

#endif // MAINWINDOW_H
