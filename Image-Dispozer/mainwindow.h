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
#include <QPropertyAnimation>
#include <QThread>

#include "imageselector.h"
#include "image_handler.h"
#include "rolestring.h"
#include "imageview.h"
#include "imagescene.h"
#include "borderrect.h"

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

    enum port_land_mode {
        PORTRAIT,
        LANDSCAPE
    };

    enum multiple_image_mode {
        SINGLE,
        MULTIPLE
    };

    port_land_mode ratio_mode;
    bool resize_on_ratio_change;
    bool reposition_on_ratio_change;
    multiple_image_mode image_mode;

    QList<image_handler * > * imageItems;
    QList<resizeRect * > * displayedItems;
    QStringList * importedImages;
    QVector<roleString * > * rolesList;

    const unsigned int item_opaque_animation_delay;
    const unsigned int border_resize_animation_delay;

    imageScene * scene;
    imageView * view;
    qreal hSize;
    qreal vSize;
    qreal offset;

    borderRect * borderRectangle;

    void initializeTreeItems(void);
    void updateRolesListWidget(void);
    void updateRolesListWidgetColor(void);
    void updateVisibleItems(void);

public slots:
    void removeAllDisplayedItems(void);
    void imageSelectorWindow(void);
    void displayNewRectItem(int row);
    void updateDisplayedItemsVector(resizeRect * item);
    void togglePortraitLandscapeMode(void);
    void toggleSingleMultipleImageMode(void);

    void hideDisplayedItems(void);
    void revealDisplayedItems(void);
};

#endif // MAINWINDOW_H
