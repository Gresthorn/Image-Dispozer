#ifndef IMAGESELECTOR_H
#define IMAGESELECTOR_H

#include <QDialog>
#include <QDebug>
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QKeyEvent>


#include "rolestring.h"
#include "image_handler.h"

namespace Ui {
class ImageSelector;
}

class ImageSelector : public QDialog
{
    Q_OBJECT

public:
    explicit ImageSelector(QVector<roleString * > * roles_list, QList<image_handler * > * image_items,
                           QStringList *imported_images, QWidget * parent = 0);
    ~ImageSelector();

private:
    Ui::ImageSelector *ui;
    QVector<roleString * > * rolesList;
    QList<image_handler * > * imageItems;
    QStringList * importedImages;

    image_handler * preview;

    void switchToAnotherParent(QTreeWidgetItem * item, int index);

protected:
    void keyPressEvent(QKeyEvent * e);

public slots:
    void linkSelectedItem(void);
    void unlinkSelectedItems(void);
    void loadSelectedImages(void);
    void loadImagesFromDir(void);
    void deleteSelectedImages(void);
    void imageListWasClicked(void);
    void imageRoleWasClicked(void);

    void cancelButtonClicked(void);
    void acceptButtonClicked(void);
};

#endif // IMAGESELECTOR_H
