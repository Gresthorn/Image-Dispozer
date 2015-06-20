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
#include <QSettings>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
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
    const qreal border_pen_width;

    imageScene * scene;
    imageView * view;
    qreal hSize;
    qreal vSize;
    qreal offset;

    borderRect * borderRectangle;

    image_handler * tempItemData;
    image_handler * lastItemDataUpdate;
    QString tempFilePath;

    QString temp_ini_file_path;

    void initializeTreeItems(void);
    void updateRolesListWidget(void);
    void updateRolesListWidgetColor(void);
    void updateVisibleItems(void);

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void removeAllDisplayedItems(void);
    void imageSelectorWindow(void);
    void initFileLoaderWindow(void);

    void displayNewRectItem(int row);
    void updateDisplayedItemsVector(resizeRect * item);
    void togglePortraitLandscapeMode(bool just_update = false);
    void toggleSingleMultipleImageMode(bool just_update = false);

    void saveSelectedItemData(image_handler * data);
    void applySavedDataSlot(void);

    void hideDisplayedItems(void);
    void revealDisplayedItems(void);

    void updateElementInfo(image_handler * item);

    void saveProfileSlot(void);
    void saveAsProfileSlot(void);
};

#endif // MAINWINDOW_H
