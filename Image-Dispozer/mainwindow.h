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
#include <QInputDialog>
#include <QDir>
#include <QFileDialog>
#include <QDirIterator>
#include <QFileInfo>
#include <QFile>
#include <QThread>
#include <QRegExp>
#include <QImageIOHandler>

#include "manualdatainput.h"
#include "imageselector.h"
#include "soundselector.h"
#include "image_handler.h"
#include "sound_handler.h"
#include "rolestring.h"
#include "imageview.h"
#include "imagescene.h"
#include "borderrect.h"
#include "infolabel.h"
#include "contactdatadialog.h"
#include "maincfgdialog.h"
#include "smscontentsdialog.h"

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

    bool something_changed;

    port_land_mode ratio_mode;
    bool resize_on_ratio_change;
    bool reposition_on_ratio_change;
    multiple_image_mode image_mode;

    QList<image_handler * > * imageItems;
    QList<sound_handler * > * soundItems;
    QList<resizeRect * > * displayedItems;
    QStringList * importedImages;
    QStringList * importedSounds;
    QVector<roleString * > * rolesList;
    QVector<roleString * > * rolesList2;

    const unsigned int item_opaque_animation_delay;
    const unsigned int border_resize_animation_delay;
    const qreal border_pen_width;

    imageScene * scene;
    imageView * view;
    int hSize;
    int vSize;
    qreal offset;

    borderRect * borderRectangle;

    image_handler * tempItemData;
    image_handler * lastItemDataUpdate;
    QString tempFilePath;

    QString temp_ini_file_path;

    QString start_sms_text;
    QString stat_sms_text;
    QString alarm_sms_text;
    QString proto_sms_text;
    bool start_sms_enabled;
    bool stat_sms_enabled;
    bool alarm_sms_enabled;
    bool proto_sms_enabled;

    QString start_sms_number;
    QString stat_sms_number;
    QString alarm_sms_number;
    QString proto_sms_number;
    QString call_number_1;
    QString call_number_2;
    QString call_number_3;
    QString call_number_4;

    int period;
    QString per_units;
    QString lan;
    bool ack_banel_error;
    int volume;

    void initializeTreeItems(void);
    bool checkCompletitionImageItems(void);
    bool checkCompletitionSoundsItems(QString * error_filepaths);
    bool checkCompletitionContacts(void);
    void updateRolesListWidget(void);
    void updateRolesListWidgetColor(void);
    void updateVisibleItems(void);

    QString createDirName(QString & path);

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void removeAllDisplayedItems(void);
    void imageSelectorWindow(void);
    void soundSelectorWindow(void);
    void initFileLoaderWindow(void);
    void contactDataWindow(void);
    void mainCfgWindow(void);
    void smsContentsWindow(void);

    void displayNewRectItem(int row);
    void updateDisplayedItemsVector(resizeRect * item);
    void togglePortraitLandscapeMode(bool just_update = false);
    void resolutionChangedSlot(bool toggled);
    void toggleSingleMultipleImageMode(bool just_update = false);
    void toggleOrthogonalRotation(bool toggle);
    void toggleResizableItems(bool toggle);

    void saveSelectedItemData(image_handler * data);
    void applySavedDataSlot(void);

    void hideDisplayedItems(void);
    void revealDisplayedItems(void);

    void updateElementInfo(image_handler * item);

    void saveProfileSlot(void);
    void saveAsProfileSlot(void);

    void updateItemRotation(void);
    void updateItemSize(void);
    void updateItemPosition(void);
    void updateItemLBCorner(void);

    void somethingChangedSlot(void) { something_changed = true;  }

    void exportDataSlot(void);
};

#endif // MAINWINDOW_H
