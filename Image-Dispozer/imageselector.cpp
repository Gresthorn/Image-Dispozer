#include "imageselector.h"
#include "ui_imageselector.h"

ImageSelector::ImageSelector(QVector<roleString * > * roles_list, QList<image_handler * > * image_items,
                             QStringList * imported_images, bool * ok, QWidget * parent) :
    QDialog(parent),
    ui(new Ui::ImageSelector)
{
    ui->setupUi(this);

    ok_s = ok;
    *ok_s = false; // default

    preview = NULL; // preview pixmap in previewLabel
    ui->previewLabel->setScaledContents(true); // pixmap will fit the size of label

    rolesList = roles_list;
    importedImages = imported_images; // we can modify the original vector since we want to store all imported image paths

    // create copy of  our images so we will not modify our previous vector in case the user clicks cancel button
    imageItems = image_items;

    QTreeWidgetItem * item;
    // go through all linked/unlinked items in vector and initialize tree widget items
    for(QList<image_handler * >::iterator it=imageItems->begin(); it!=imageItems->end(); it++)
    {
        if((*it)->isFileCorrect())
            item = new QTreeWidgetItem(ui->rolesTree->topLevelItem(0), QStringList() << *rolesList->at((*it)->getIndex()));
        else
            item = new QTreeWidgetItem(ui->rolesTree->topLevelItem(1), QStringList() << *rolesList->at((*it)->getIndex()));
        item->setData(1, Qt::UserRole, (*it)->getIndex());
    }

    // go through all imported file history and fill the list of imported files with the correct item
    // create items in list widgets
    QString couldNotBeLoaded;
    int i = 0;
    while(i<importedImages->count())
    {
        QFileInfo curr_item(importedImages->at(i++));
        // add new widget to the list. If file was removed or is not at the specified position
        if(!curr_item.exists())
        {
            couldNotBeLoaded.append(QString("\n%1").arg(curr_item.fileName()));
            importedImages->removeAt(--i); // delete unactual item from the list
        }
        else
        {
            QListWidgetItem * item = new QListWidgetItem(curr_item.fileName());
            item->setData(Qt::UserRole, importedImages->at(i-1)); // save path to the image as data
            ui->importedImagesList->addItem(item); // create new item in list
        }
    }

    // make signal-slot connection for buttons (unlink, link, load images etc)
    connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(linkSelectedItem()));
    connect(ui->unlinkButton, SIGNAL(clicked()), this, SLOT(unlinkSelectedItems()));
    connect(ui->selectImageButton, SIGNAL(clicked()), this, SLOT(loadSelectedImages()));
    connect(ui->removeSelectedButton, SIGNAL(clicked()), this, SLOT(deleteSelectedImages()));
    connect(ui->selectDirectoryButton, SIGNAL(clicked()), this, SLOT(loadImagesFromDir()));
    connect(ui->importedImagesList, SIGNAL(itemSelectionChanged()),
            this, SLOT(imageListWasClicked()));
    connect(ui->rolesTree, SIGNAL(itemSelectionChanged()), this, SLOT(imageRoleWasClicked()));
    connect(this, SIGNAL(rejected()), this, SLOT(cancelButtonClicked()));
    connect(this, SIGNAL(accepted()), this, SLOT(acceptButtonClicked()));

    if(!couldNotBeLoaded.isEmpty())
    {
        this->show();
        QMessageBox::warning(this, tr("The following files could not be loaded"), couldNotBeLoaded, QMessageBox::Ok);
    }
}

ImageSelector::~ImageSelector()
{
    delete ui;
}

void ImageSelector::switchToAnotherParent(QTreeWidgetItem *item, int index)
{
    // if item is top level item, do nothing
    if(item->parent()==NULL) return;

    // if item already is at parent with 'index', do nothing
    if(item->parent()==ui->rolesTree->topLevelItem(index)) return;

    // find appropriate position for item. If index data is greater than the item's,
    // remember position and insert new item at this position
    int i;
    for(i=0; i<ui->rolesTree->topLevelItem(index)->childCount(); i++)
    {
        if(item->data(1, Qt::UserRole).toInt()<
                ui->rolesTree->topLevelItem(index)->child(i)->data(1, Qt::UserRole).toInt()) break;
    }

    ui->rolesTree->topLevelItem(index)->insertChild(i, item->clone());
    delete item; // previous item is no more needed
}

void ImageSelector::keyPressEvent(QKeyEvent *e)
{
    if(e->modifiers()==Qt::ControlModifier)
    {
        // if CTRL+B is pressed try to link selected items
        if(e->key()==Qt::Key_B)
            linkSelectedItem();
        // if CTRL+N is pressed try to unlink selected items
        else if(e->key()==Qt::Key_N)
            unlinkSelectedItems();
        // if CTRL+X is pressed try to remove selected image items
        else if(e->key()==Qt::Key_X)
            deleteSelectedImages();
    }

    QDialog::keyPressEvent(e);
}

void ImageSelector::linkSelectedItem()
{
    if(!ui->rolesTree->selectedItems().isEmpty() && !ui->importedImagesList->selectedItems().isEmpty())
    {
        if(ui->rolesTree->selectedItems().size()>1 || ui->importedImagesList->selectedItems().size()>1)
            QMessageBox::warning(this, tr("Too many items"), tr("Select only one item in roles tree and image list to link."),
                                              QMessageBox::Ok);
        else
        {
            // do nothing if top level item is selected, or item in linked category is highlighted
            if(ui->rolesTree->selectedItems().first()->parent()==NULL ||
                    ui->rolesTree->selectedItems().first()->parent()==ui->rolesTree->topLevelItem(0)) return;

            // get the image path and set the new image to appropriate object
            QString path = ui->importedImagesList->selectedItems().first()->data(Qt::UserRole).toString();
            int index = ui->rolesTree->selectedItems().first()->data(1, Qt::UserRole).toInt();

            // set new image
            // imageItems->at(index)->setImage(path); // OLD CODE
            imageItems->at(index)->setTempImagePath(path);

            // if incremental switching is allowed, we need to remember the index of selected item
            int curr_position_role = 0;
            int curr_position_item = 0; // image item;
            if(ui->incrementalSwitchCheckbox->isChecked())
            {
                curr_position_role = ui->rolesTree->topLevelItem(1)->indexOfChild(ui->rolesTree->selectedItems().first());
                curr_position_item = ui->importedImagesList->row(ui->importedImagesList->selectedItems().first());
            }

            // migrate the item to linked category
            switchToAnotherParent(ui->rolesTree->selectedItems().first(), 0);

            // remove image record in image list
            delete ui->importedImagesList->selectedItems().first();

            // highlight (select) the nearest item to the lastly selected and linked item
            if(ui->incrementalSwitchCheckbox->isChecked())
            {
                int items_count = ui->rolesTree->topLevelItem(1)->childCount();

                // no more items to link
                if(items_count!=0)
                {
                    if(items_count==curr_position_role)
                        curr_position_role = 0; // we reached end of list, so we go back to the beginning

                    ui->rolesTree->topLevelItem(1)->child(curr_position_role)->setSelected(true);
                }

                // do the same logic for image items list
                int images_count = ui->importedImagesList->count();

                // no more items to link in image list
                if(images_count!=0)
                {
                    if(images_count==curr_position_item)
                        curr_position_item = 0; // we reached end of list, returning to the first item

                    ui->importedImagesList->item(curr_position_item)->setSelected(true);
                }
            }
        }
    }
    else QMessageBox::warning(this, tr("Invalid selection"), tr("There is no role or image selected for link operation."),
                              QMessageBox::Ok);
}

void ImageSelector::unlinkSelectedItems()
{   
    if(!ui->rolesTree->selectedItems().isEmpty())
    {
        foreach(QTreeWidgetItem * item, ui->rolesTree->selectedItems())
        {
            // do nothing if top level item is selected, or item in unlinked category is highlighted
            if(item->parent()==NULL || item->parent()==ui->rolesTree->topLevelItem(1)) continue;

            // discard initial image, so object is empty again
            int index = item->data(1, Qt::UserRole).toInt();
            QString img_path = imageItems->at(index)->getTempImagePath();

            qDebug() << img_path;

            imageItems->at(index)->clearTempImagePath();
            // imageItems->at(index)->discardImage(); // OLD CODE

            // if incremental switching is allowed, we need to remember the index of selected item
            int curr_position_role = 0;
            if(ui->incrementalSwitchCheckbox->isChecked())
            {
                curr_position_role = ui->rolesTree->topLevelItem(0)->indexOfChild(ui->rolesTree->selectedItems().first());
            }

            // migrate the item to unlinked category
            switchToAnotherParent(item, 1);

            // add released image back to image list
            QFileInfo curr_file(img_path);
            if(curr_file.exists())
            {
                // if image still exists, we can return the record about it back to the image list
                QListWidgetItem * item = new QListWidgetItem(curr_file.fileName());
                item->setData(Qt::UserRole, img_path); // save path to the image as data
                ui->importedImagesList->addItem(item); // create new item in list
            }
            else QMessageBox::warning(this, tr("Image cannot be found"), tr("During attempt to return image back into "
                                                                            "file list program found, that image is not "
                                                                            "longer at given location. Image was discarded."),
                                      QMessageBox::Ok);

            // highlight (select) the nearest item to the lastly selected and linked item
            if(ui->incrementalSwitchCheckbox->isChecked())
            {
                int items_count = ui->rolesTree->topLevelItem(0)->childCount();

                // no more items to link
                if(items_count!=0)
                {
                    if(items_count==curr_position_role)
                        curr_position_role = 0; // we reached end of list, so we go back to the beginning

                    ui->rolesTree->topLevelItem(0)->child(curr_position_role)->setSelected(true);
                }
            }
        }
    }
    else QMessageBox::warning(this, tr("No selection"), tr("There are no items selected for unlink operation."),
                              QMessageBox::Ok);
}

void ImageSelector::loadSelectedImages()
{
    QStringList selectedFiles = QFileDialog::getOpenFileNames(this, tr("Select images to load"), QDir::currentPath(),
                                                                QString("Image files (*.bmp)"));
    // if user clicks cancel, or selects no items
    if(selectedFiles.isEmpty()) return;

    // create items in list widgets
    foreach(QString file, selectedFiles)
    {
        QFileInfo curr_item(file);
        // add new widget to the list
        QListWidgetItem * item = new QListWidgetItem(curr_item.fileName());
        item->setData(Qt::UserRole, file); // save path to the image as data
        ui->importedImagesList->addItem(item);
        importedImages->append(file);
    }
}

void ImageSelector::loadImagesFromDir()
{
    QDir selected_dir;
    selected_dir.setPath(QFileDialog::getExistingDirectory(this, tr("Select folder with images"), QDir::currentPath(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));

    // if user clicks cancel button, returned string is empty
    if(selected_dir.path().isEmpty()) return;

    // Iterate over all files in directory, so we can check if they are images.
    // I decided to not recursively iterate into subdirectories.
    QDirIterator it(selected_dir.path(), QDir::Files, QDirIterator::NoIteratorFlags);
    while(it.hasNext())
    {
        // check if is image in format bmp
        QFileInfo file(it.next());
        if(file.suffix()=="bmp")
        {
            // file is image, so we can add it to the imported list
            QListWidgetItem * item = new QListWidgetItem(file.fileName());
            item->setData(Qt::UserRole, file.filePath()); // save path to the image as data
            ui->importedImagesList->addItem(item);
            importedImages->append(file.filePath());
        }
    }
}

void ImageSelector::deleteSelectedImages()
{
    foreach(QListWidgetItem * selected, ui->importedImagesList->selectedItems())
    {
        // every selected item in imported images list will be immediately removed from list and deleted
        delete selected;
    }
}

void ImageSelector::imageListWasClicked(void)
{
    // check if there is only one item selected
    if(ui->importedImagesList->selectedItems().count()>1 || ui->importedImagesList->selectedItems().isEmpty())
    {
        // cannot display more items at once - display informative text
        ui->previewLabel->setText(tr("no preview"));
    }
    else
    {
        if(preview!=NULL)
        {
            delete preview;
            preview = NULL;
        }

        QString path = ui->importedImagesList->selectedItems().first()->data(Qt::UserRole).toString();

        preview = new image_handler(path, -1, -1); // preview can be displayed also without role code or index
        if(preview->isFileCorrect())
        {
            ui->previewLabel->setPixmap(*preview);
        }
        else
        {
            // reset preview and inform user about problem
            ui->previewLabel->setText(tr("no preview"));

            QMessageBox::warning(this, tr("Image could not be loaded"),
                                  tr("The selected image could not be loaded. Check if the image is still availible on "
                                     "your disk or is in correct format (bmp)."), QMessageBox::Ok);
        }
    }
}

void ImageSelector::imageRoleWasClicked()
{
    // check if there is only one item selected
    if(ui->rolesTree->selectedItems().count()>1 || ui->rolesTree->selectedItems().isEmpty())
    {
        // cannot display more items at once - display informative text
        ui->previewLabel->setText(tr("no preview"));
    }
    else
    {
        // if the selected item is not linked item, or it is top level item, do nothing
        if(ui->rolesTree->selectedItems().first()->parent()==NULL ||
           ui->rolesTree->selectedItems().first()->parent()==ui->rolesTree->topLevelItem(1))
        {
            ui->previewLabel->setText(tr("no preview"));
            return;
        }

        int index = ui->rolesTree->selectedItems().first()->data(1, Qt::UserRole).toInt();

        if(preview!=NULL)
        {
            delete preview;
            preview = NULL;
        }

        QString path = imageItems->at(index)->getTempImagePath();
        if(path.isEmpty())
        {
            ui->previewLabel->setText(tr("no preview"));
            return;
        }

        preview = new image_handler(path, -1, -1); // preview can be displayed also without role code or index
        // if image is not correctly loaded, it may be usefull to inform the user about error
        if(preview->isFileCorrect())
        {
            ui->previewLabel->setPixmap(*preview);
        }
        else
        {
            // reset preview and inform user about problem
            ui->previewLabel->setText(tr("no preview"));

            QMessageBox::warning(this, tr("Image could not be loaded"),
                                  tr("The selected image could not be loaded. Check if the image is still availible on "
                                     "your disk or is in correct format (bmp)."), QMessageBox::Ok);
        }
    }
}

void ImageSelector::cancelButtonClicked()
{
    // if user clicked cancel button, we must discard temporary paths
    for(QList<image_handler * >::iterator it=imageItems->begin(); it!=imageItems->end(); it++)
        (*it)->clearTempImagePath();
}

void ImageSelector::acceptButtonClicked()
{
    QString result;

    // append new items to original list
    for(QList<image_handler * >::iterator it=imageItems->begin(); it!=imageItems->end(); it++)
    {
        // check if there is some image to be loaded
        if((*it)->getTempImagePath().isEmpty())
        {
            (*it)->discardImage(); // delete image, if there is some
            continue;
        }

        int loading_result = (*it)->setImage((*it)->getTempImagePath());
        if(loading_result!=1) // an error occured when loading image
        {
            result.append(QString("The file: \"%1\" could not be loaded").arg((*it)->getTempImagePath()));

            if(loading_result==0)
                result.append(" but the old image was <b>successfuly</b> refreshed.");
            else result.append(" but the old image <b>could not be</b> refreshed.");

            result.append("\n");
        }
    }

    // clear importedImages vector, we will fill it with new values
    importedImages->clear();
    for(int i=0; i<ui->importedImagesList->count(); i++)
    {
        QListWidgetItem * temp = ui->importedImagesList->item(i);
        importedImages->append(temp->data(Qt::UserRole).toString());
    }

    // if there is some error message to be displayed
    if(!result.isEmpty()) QMessageBox::warning(this, tr("Some images could not be loaded"), result, QMessageBox::Ok);

    *ok_s = true;
}
