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
#include "apnsettingsdialog.h"
#include "exportselectordialog.h"

namespace Ui {
class MainWindow;
}

/*! \class MainWindow mainwindow.h
 * \brief Class responsible for showing the initial window of application.
 *
 * This class is the main class of application. Consists of methods responsible for
 * showing up the initial window for application and runs algorithms for evoking other
 * subwindows, dialogs etc. The MainWindow class contains vectors for storing loaded
 * pixmaps and imported images/sounds paths. Also displays graphics scene and provides
 * interface for disposing images and groups of images to desired positions. Those data
 * are then stored in image_handler class from where they are exported to text files.
 * Note that only image_handler based objects are used for export or later load data.
 * Each change in graphics scene must reflect to vector with image_handler objects.
 *
 * All image_handler objects are stored in MainWindow::imageItems while sounds in MainWindow::soundItems list.
 * If the object which is member of group X is changed, also other group members must be
 * updated with the same parameters. Possible groups are stored in #image_groups enumeration
 * type and passed to image_handler so the group identifier can be easily read by image_handler::getImageGroup()
 * function. Each image has a role assigned as well. The role represents image/sound original
 * meaning in target system. Therefore only one image/sound can be assigned to one specific
 * role. All possible image roles codes and their text interpretations are availible in MainWindow#rolesList while
 * sound roles are present in MainWindow#rolesList2.
 *
 * Program is able to use two possible resolutions so far. 320x240 and 480x320. Both can
 * be used in portrait or landscape mode. Currently used mode is set in #ratio_mode. Since
 * resulting coordinates are expressed in one common coordinate system, one of the modes
 * must be set as initial or base mode. This setting is availible in #baseMode variable.
 * Coordinates for the another mode (other from the base mode) are calculated by rotating
 * the base mode display to the right or to the left. This rotation is specified in #modeChangeRotDirection variable.
 *
 * This class also provides a basic methods for updating values of image_handler objects
 * as well as methods and slots for communication with the scene and view objects. Also
 * all configurable data are stored here, so after loading dialog window for different
 * settings again, the lastly saved information can be easily accessed. From main window
 * class it is also possible to save or load data. Those can be stored in simple INI file.
 * The function responsible for exporting data is MainWindow::exportDataSlot(). For loading
 * sequence, see the MainWindow::
 *
 */

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*! \fn explicit MainWindow(QWidget *parent = 0);
     * \brief This is the constructor of MainWindow class. It is responsible for all initialization of program.
     * \param parent Parent widget for MainWindow.
     *
     * In this constructor, all main initialization of application is done as well as new
     * vectors and lists are created. Constructor also calls MainWindow::initializeTreeItems() function
     * for filling the MainWindow#rolesList and MainWindow#rolesList2 with correct roles and their
     * codes. After function is finished, also image_handler and sound_handler lists are prepared
     * for use. Constructor will also build the imageScene and imageView objects.
     *
     *  See also:
     *  MainWindow::initializeTreeItems()
     *  imageScene
     *  imageView
     */
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    enum port_land_mode {
        PORTRAIT,
        LANDSCAPE,
        UNDEFINED
    };

    enum multiple_image_mode {
        SINGLE,
        MULTIPLE
    };

    enum port_land_rot_tran_mode {
        LEFT,
        RIGHT
    };

    port_land_mode baseMode;
    port_land_mode ratio_mode;
    port_land_rot_tran_mode modeChangeRotDirection;

    bool something_changed;

    bool resize_on_ratio_change;
    bool reposition_on_ratio_change;
    multiple_image_mode image_mode;

    //! A list storing all image_handler objects in one place for easy access
    /*!
     * This is the list of all image_handler objects availible in program. These
     * objects are used for storing all data of specific image object. The list
     * is initialized once in constructor and must not be changed anywhere else
     * in program. If new items are needed in the list, update MainWindow::initializeTreeItems()
     * function.
     *
     * See also:
     * image_handler
     */
    QList<image_handler * > * imageItems;

    //! A list storing all sound_handler objects in one place for easy access
    /*!
     * This is the list of all sound_handler objects availible in program. These
     * objects are used for storing all data of specific sound element. The list
     * is initialized once in constructor and must not be changed anywhere else
     * in program. If new items are needed in the list, update MainWindow::initializeTreeItems()
     * function.
     *
     * See also:
     * sound_handler
     */
    QList<sound_handler * > * soundItems;

    //! This list contains all items currently shown in the scene.
    /*!
     * After the user double clicks the handler successfuly associated with appropriate image,
     * the image with its current data will be displayed in the scene. For better management,
     * these items are stored here so they can be deleted or hidden when needed.
     *
     * See also:
     * MainWindow::updateVisibleItems()
     * MainWindow::removeAllDisplayedItems();
     */
    QList<resizeRect * > * displayedItems;

    //! This list holds the paths to imported images.
    /*!
     * As far as user can still return to the imageSelector or soundSelector window, imported
     * images should be refreshed from his last visit. After leaving imageSelector window all
     * image paths are stored here for later refresh.
     *
     * See also:
     * imageSelector
     */
    QStringList * importedImages;

    //! This list holds the paths to imported images.
    /*!
     * As far as user can still return to the imageSelector or soundSelector window, imported
     * sounds should be refreshed from his last visit. After leaving soundSelector window all
     * sound paths are stored here for later refresh.
     *
     * See also:
     * soundSelector
     */
    QStringList * importedSounds;

    //! The vector holds all possible image roles availible in application.
    /*!
     * The vector is fulfilled in constructor of MainWindow class. This vector is not allowed to
     * change elsewhere in the program since it must contain all possible roles or images/sounds
     * to be able exported by program. Note that according to this vector the MainWindow::imageItems
     * list is filled. This list contains the objects of roleString class which can store both:
     * the name of role and it's representation code.
     *
     * See also:
     * MainWindow#imageItems
     * roleString
     * MainWindow#rolesList2
     */
    QVector<roleString * > * rolesList;

    //! The vector holds all possible sound roles availible in application.
    /*!
     * The vector is fulfilled in constructor of MainWindow class. This vector is not allowed to
     * change elsewhere in the program since it must contain all possible roles or images/sounds
     * to be able exported by program. Note that according to this vector the MainWindow::imageItems
     * list is filled. This list contains the objects of roleString class which can store both:
     * the name of role and it's representation code.
     *
     * See also:
     * MainWindow#imageItems
     * roleString
     * MainWindow#rolesList
     */
    QVector<roleString * > * rolesList2;

    //! This constant specify the speed of opacity change animation of items.
    /*!
     * This constant is used for setting the animation delay of disappearing and revealing the items
     * in scene during changing the portrait to landscape mode (or vice versa). This animation has
     * only esthetic function.
     *
     * See also:
     * MainWindow::togglePortraitLandscapeMode(bool);
     */
    const unsigned int item_opaque_animation_delay;

    //! The constant sets up the animation delay of changing scene border size.
    /*!
     * After user clicks the button for changing portrait to the landscape mode (or vice versa) the main
     * scene border should change it's width and height accordingly. This change is done by animation which
     * duration is given by this constant.
     *
     * See also:
     * MainWindow::togglePortraitLandscapeMode(bool);
     */
    const unsigned int border_resize_animation_delay;

    //! The constant specifies the pen width of main scene border.
    const qreal border_pen_width;

    //! This object represents the main scene of items being displayed for user.
    /*!
     * This object is based on imageScene class which inherits the QGraphicsScene class. The object behaves
     * like container for all currently displayed items. Object can however contain other items than resizeRect
     * as well (e.g. border rectangle) so filters are needed when iterating over all items. The scene also contains
     * some additional information like: lastly set z-index, dimensions of scene, #sceneItemsMode etc. needed
     * by view or main window in different circumstances. Do not forget to update those values in the scene as well
     * (not in MainWindow only).
     *
     * See also:
     * imageScene
     * imageView
     */
    imageScene * scene;

    //! This object represents the main visualization object responsible for drawing items on the screen.
    /*!
     * The view object is based on class imageView which inherits the QGraphicsView class. For more information
     * about this class, see the official Qt documentation. Additional functionality takes place in signaling
     * the changed position, rotation of item for MainWindow. Also is doing some stretching of items and establishes
     * the context menu with options for saving data, hiding selected items etc. For more information see imageView
     * class documentation.
     *
     * See also:
     * imageView
     * imageScene
     */
    imageView * view;

    //! This integer variable stores the height of working area.
    /*!
     * The working area is space for dispozing images. In result it is the final display where the images will be
     * displayed on target devices. Possible values are 240 or 320. The value is integer since it represents the
     * number of pixels. However during many math operations it must be converted to qreal since positions and
     * sizes of graphics items are expressed as such. Note that this value is also present in imageView and imageScene
     * and therefore after changing in MainWindow, also change in those objects must be done.
     *
     * See also:
     * MainWindow#vSize
     */
    int hSize;

    //! This integer variable stores the width of working area.
    /*!
     * The working area is space for dispozing images. In result it is the final display where the images will be
     * displayed on target devices. Possible values are 320 or 480. The value is integer since it represents the
     * number of pixels. However during many math operations it must be converted to qreal since positions and
     * sizes of graphics items are expressed as such. Note that this value is also present in imageView and imageScene
     * and therefore after changing in MainWindow, also change in those objects must be done.
     *
     * See also:
     * MainWindow#hSize
     */
    int vSize;

    //! This value represents the offset added to the scene's renderable rectangle.
    /*!
     * The offset value is used for adding it to the scene renderable rectangle sizes so full scene rect
     * is correctly displayed. Also when resizing window, the scroll bars will appear sooner.
     */
    qreal offset;

    //! Border rectangle around the working area.
    /*!
     * This rectangle is added to the scene to visually separate the working area from the rest of
     * visible scene. Note that this rectangle is only visual addition, it does not block the items from
     * leaving the working area. This functionality is done by other functions in imageView and resizeRect
     * based objects.
     *
     * Althought the simple rectangle is displayed, the class is modified for animation support.
     */
    borderRect * borderRectangle;

    //! This is only temporary image_handler object that serves as data container when copying parameters.
    /*!
     * This object servers as temporary data storage. When user clicks the option to copy data of selected
     * item in the scene, these are stored here until other data will overwrite them. When applying saved
     * data on other objects, this object is used as a template.
     *
     * See also:
     * MainWindow::saveSelectedItemData(image_handler *)
     * MainWindow::applySavedDataSlot()
     */
    image_handler * tempItemData;

    //! Pointer to the lastly updated item which allows not to update unnecessary labels.
    /*!
     * By holding lastly updated item pointer, program can check for this pointer when updating labels for the
     * user and prevent from updating labels that cannot change for the item (role name, image name etc.). After
     * user clicks another item the condition will not match and all labels will be updated.
     *
     * See also:
     * MainWindow::updateElementInfo(image_handler *)
     */
    image_handler * lastItemDataUpdate;

    //! Path to INI file which was lastly used for export.
    /*!
     * In this variable, the path to the lastly used INI file is stored. When user export data to some, or loads
     * data from the INI file, it's filepath is stored here so simple 'save' option from main menu can be used for
     * quick save. If this string is empty, even quick save option will evoke a dialog asking for new path for new
     * INI file.
     *
     * See also:
     * MainWindow::saveProfileSlot()
     * MainWindow::saveAsProfileSlot()
     */
    QString temp_ini_file_path;

    dataToExport data_to_export;

    //! Data holder variable for device start sms text.
    QString start_sms_text;
    //! Data holder variable for device status sms text.
    QString stat_sms_text;
    //! Data holder variable for alarm sms text.
    QString alarm_sms_text;
    //! Data holder variable for protocol sms text.
    QString proto_sms_text;
    //! Data holder variable for enabling or disabling sending start sms.
    bool start_sms_enabled;
    //! Data holder variable for enabling or disabling sending status sms.
    bool stat_sms_enabled;
    //! Data holder variable for enabling or disabling sending alarm sms.
    bool alarm_sms_enabled;
    //! Data holder variable for enabling or disabling sending protocol sms.
    bool proto_sms_enabled;

    //! The string with tel. number where start sms are to be sent.
    QString start_sms_number;
    //! The string with tel. number where status sms are to be sent.
    QString stat_sms_number;
    //! The string with tel. number where alarm sms are to be sent.
    QString alarm_sms_number;
    //! The string with tel. number where protocol sms are to be sent.
    QString proto_sms_number;
    //! The string containing tel. number used for GSM call in emergency situation.
    QString call_number_1;
    //! The string containing tel. number used for GSM call in emergency situation as an alternative for first number.
    QString call_number_2;
    //! The string containing tel. number used for GSM call in emergency situation as an alternative for second number.
    QString call_number_3;
    //! The string containing tel. number used for GSM call in emergency situation as an alternative for third number.
    QString call_number_4;

    //! Time period or frequency of sms sending measured in MainWindow#per_units.
    int period;
    //! Units of time period defining the frequency of sms sending. Final frequency is defined together with MainWindow#period .
    QString per_units;
    //! ???
    QString lan;
    //! ???
    bool ack_banel_error;
    //! Volume level of GSM call to number MainWindow#call_number_1 or alternative numbers.
    int volume;

    //! Name of network or access point needed for establish connection to network.
    QString ap_name;
    //! Login for accessing the network services.
    QString login;
    //! Password for accessing the network services.
    QString password;
    //! Boolean value enabling or disabling access with password.
    bool request_password;
    //! Enabling or disabling using proxy.
    bool use_proxy;
    //! Selector of address mode which can be passed as IP address or common url (simple text format).
    address_mode addr_mode;
    //! Address of server to connect.
    QString address;
    //! IP address of primary DNS server.
    QString DNS1;
    //! IP address of secondary DNS server.
    QString DNS2;


    /*! \fn void setHandlerRealPosition(int, int, resizeRect *)
     * \brief Function for setting the image position by corrections directly on resizeRect object.
     *
     * This function can be used to set up the image position when there is availible resizeRect object
     * in scene and we have a pointer to it. Function will update item's position and do corrections if
     * item is leaving working area. Then external data in image_handler pair are correctly updated together
     * with left bottom corner position.
     *
     * See also:
     * MainWindow::setHandlerRealPosition(qreal, qreal, image_handler *)
     */
    void setHandlerRealPosition(int x, int y, resizeRect *item);

    /*! \fn void setHandlerRealPosition(qreal, qreal, image_handler *)
     * \brief Function for setting the image position by corrections directly on resizeRect object.
     *
     * This function can be used to set up the image position when there is availible image_handler object
     * pointer. Since the image_handler object does not necessarily have to own any resizeRect object, the
     * position prediction and correction must be done by another methods than in case of resizeRect overloaded
     * function. This can be achieved by image_handler::calculateWrapperCorners() and MainWindow::checkIfInside(image_handler *)
     * methods. Left bottom corner and position correction are part of MainWindow::checkIfInside(image_handler *) method.
     *
     * See also:
     * MainWindow::setHandlerRealPosition(int, int, resizeRect *)
     * MainWindow::checkIfInside(image_handler *)
     * image_handler::calculateWrapperCorners()
     */
    void setHandlerRealPosition(qreal x, qreal y, image_handler * item);

    /*! \fn void setHandlerRealPosition(qreal, qreal, image_handler *)
     * \brief Function for setting the image position by corrections directly on resizeRect object.
     *
     * This function can be used to set up the image position when there is availible image_handler object
     * pointer. Since the image_handler object does not necessarily have to own any resizeRect object, the
     * position prediction and correction must be done by another methods than in case of resizeRect overloaded
     * function. This can be achieved by image_handler::calculateWrapperCorners() and MainWindow::checkIfInside(image_handler *)
     * methods. Left bottom corner and position correction are part of MainWindow::checkIfInside(image_handler *) method.
     *
     * See also:
     * MainWindow::setHandlerRealPosition(int, int, resizeRect *)
     * MainWindow::setHandlerRealLBCorner(int, int, resizeRect *)
     * MainWindow::checkIfInside(image_handler *)
     * image_handler::calculateWrapperCorners()
     */
    bool checkIfInside(image_handler * item);

    /*! \fn bool predictRotation(image_handler *, qreal)
     * \brief Function for wrapper calculation and scene leaving correction when item is rotated.
     *
     * This method is used for wrapper calculations when item is rotated. It uses the image_handler::calculateWrapperCorners(qreal)
     * method for calculating the wrapper rectangle before the angle is set. If wrapper is inside working area, function returns
     * true. In other case function returns false. Note that this function serves only for prediction of next state after rotation
     * not for updating the values in image_handler.
     *
     * See also:
     * image_handler::calculateWrapperCorners(qreal)
     */
    bool predictRotation(image_handler * item, qreal angle);

    /*! \fn void setHandlerRealLBCorner(int, int, resizeRect *)
     * \brief Function for setting the image item position by giving bottom left corner.
     *
     * This method is used for setting the resizeRect item's position by giving a bottom left corner instead of central [x, y] position.
     * Method will calculate the wrapper and sets the position of bottom left corner. Central position is calculated. If item is
     * leaving the working area, method will try to find the nearest x and y coordinate to the edge of scene, where the item is
     * leaving working area. Method will also automatically update image_handler data as well as resizeRect data.
     *
     * See also:
     * image_handler::calculateWrapperCorners(qreal)
     * MainWindow::checkIfInside(image_handler *)
     * MainWindow::setHandlerRealPosition(int, int, resizeRect *)
     */
    void setHandlerRealLBCorner(int x, int y, resizeRect *item);

    //void setHandlerRealSize(int w, int h, image_handler * item);

    /*! \fn void initializeTreeItems(void)
     * \brief Initialization of roles, image_handler and sound_handler objects lists.
     *
     * This function is called in constructor of MainWindow. It can be called only once
     * in program since it's purpose is to initialize MainWindow#rolesList and MainWindow#rolesList2
     * lists. These are using roleString class which inherits the QString but can store the text
     * and code interpretation of specific role together. After all roles are loaded, the lists
     * MainWindow#imageItems and MainWindow#soundItems are created and filled with objects for
     * each role. Finally MainWindow::updateRolesListWidget() is called to update color and icons of for
     * user visible items in QTreeWidgetItems.
     *
     * See also:
     * MainWindow::updateRolesListWidget()
     */
    void initializeTreeItems(void);
    bool checkCompletitionImageItems(void);
    bool checkCompletitionNetworkConfig(void);
    bool checkCompletitionSoundsItems(QString * error_filepaths);
    bool checkCompletitionContacts(void);
    void updateRolesListWidget(void);
    void updateRolesListWidgetColor(void);
    void clearRolesListChildItems(void);
    void updateVisibleItems(image_groups group=NONE);
    QPointF calculateRealCoordinates(const QPointF & pos);
    qreal calculateRealAngle(const qreal & angle);

    QString createDirName(QString & path);

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void debugSlot(void);

    void removeAllDisplayedItems(void);
    void imageSelectorWindow(void);
    void soundSelectorWindow(void);
    void initFileLoaderWindow(void);
    void contactDataWindow(void);
    void mainCfgWindow(void);
    void smsContentsWindow(void);
    void apnConfigurationWindow(void);
    void selectExportsWindow(void);

    void displayNewRectItem(QTreeWidgetItem * item, int row, bool displayComposition = false);
    void displayCompositionSlot(void);
    void updateDisplayedItemsVector(resizeRect * item);
    void togglePortraitLandscapeMode(bool just_update = false);
    void transformationBaseModeChange(bool toggle);
    void transformationRotationModeChange(bool toggle);
    void resolutionChangedSlot(bool toggled);
    void toggleSingleMultipleImageMode(bool just_update = false);
    void toggleOrthogonalRotation(bool toggle);
    void toggleResizableItems(bool toggle);
    void toggleResizeOnRatioChange(bool toggle);
    void toggleRepositionOnRatioChange(bool toggle);

    void saveSelectedItemData(image_handler * data);
    void applySavedDataSlot(void);

    void hideDisplayedItems(void);
    void revealDisplayedItems(void);

    void updateElementInfo(image_handler * item);
    void updateGroupPosData(image_handler * item);
    void updateGroupRotData(image_handler * item);
    void updateItemsDockStateIcon(image_handler * item);

    void saveProfileSlot(void);
    void saveAsProfileSlot(void);

    void updateItemRotation(void);
    //void updateItemSize(void);
    void updateItemPosition(void);
    void updateItemLBCorner(void);

    void somethingChangedSlot(void) { something_changed = true;  }

    void exportDataSlot(void);

    void aboutSlot(void);
};

#endif // MAINWINDOW_H
