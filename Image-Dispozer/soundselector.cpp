#include "soundselector.h"
#include "ui_soundselector.h"

SoundSelector::SoundSelector(QVector<roleString * > * roles_list, QList<sound_handler * > * sound_items,
                             QStringList * imported_sounds, bool * ok, QWidget * parent) :
    QDialog(parent),
    ui(new Ui::SoundSelector)
{
    ui->setupUi(this);

    default_volume_level = 5;
    ui->volumeSlider->setValue(default_volume_level);
    ui->volumeLabel->setText(QString("Volume (%1):").arg(default_volume_level));
    preview = original_handler = NULL;
    mediaPlayer = new QMediaPlayer(this);
    mediaPlayer->setNotifyInterval(100);
    mediaPlayer->setVolume(default_volume_level*10);
    disablePlayer();

    ok_s = ok;
    *ok_s = false; // default

    rolesList = roles_list; // in main windows this is roleList2 - list of sounds
    importedSounds = imported_sounds; // we can modify the original vector since we want to store all imported sound paths

    // create copy of  our sounds so we will not modify our previous vector in case the user clicks cancel button
    soundItems = sound_items;

    QTreeWidgetItem * item;
    // go through all linked/unlinked items in vector and initialize tree widget items
    for(QList<sound_handler * >::iterator it=soundItems->begin(); it!=soundItems->end(); it++)
    {

        if(!(*it)->isEmpty())
            item = new QTreeWidgetItem(ui->rolesTree->topLevelItem(0), QStringList() << *rolesList->at((*it)->getIndex()));
        else
            item = new QTreeWidgetItem(ui->rolesTree->topLevelItem(1), QStringList() << *rolesList->at((*it)->getIndex()));
        item->setData(1, Qt::UserRole, (*it)->getIndex());
    }

    // go through all imported file history and fill the list of imported files with the correct item
    // create items in list widgets
    QString couldNotBeLoaded;
    int i = 0;
    while(i<importedSounds->count())
    {
        QFileInfo curr_item(importedSounds->at(i++));
        // add new widget to the list. If file was removed or is not at the specified position
        if(!curr_item.exists())
        {
            couldNotBeLoaded.append(QString("\n%1").arg(curr_item.fileName()));
            importedSounds->removeAt(--i); // delete unactual item from the list
        }
        else
        {
            QListWidgetItem * item = new QListWidgetItem(curr_item.fileName());
            item->setData(Qt::UserRole, importedSounds->at(i-1)); // save path to the image as data
            ui->importedSoundsList->addItem(item); // create new item in list
        }
    }

    // make signal-slot connection for buttons (unlink, link, load images etc)
    connect(ui->linkButton, SIGNAL(clicked()), this, SLOT(linkSelectedItem()));
    connect(ui->unlinkButton, SIGNAL(clicked()), this, SLOT(unlinkSelectedItems()));
    connect(ui->selectSoundButton, SIGNAL(clicked()), this, SLOT(loadSelectedSounds()));
    connect(ui->removeSelectedButton, SIGNAL(clicked()), this, SLOT(deleteSelectedSounds()));
    connect(ui->selectDirectoryButton, SIGNAL(clicked()), this, SLOT(loadSoundsFromDir()));
    connect(ui->importedSoundsList, SIGNAL(itemSelectionChanged()),
            this, SLOT(soundListWasClicked()));
    connect(ui->rolesTree, SIGNAL(itemSelectionChanged()), this, SLOT(soundRoleWasClicked()));
    connect(this, SIGNAL(rejected()), this, SLOT(cancelButtonClicked()));
    connect(this, SIGNAL(accepted()), this, SLOT(acceptButtonClicked()));

    connect(mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(progressChangedSlot(qint64)));
    connect(ui->progressSlider, SIGNAL(sliderMoved(int)), this, SLOT(progressUserChangedSlot(int)));
    connect(mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(durationChangedSlot(qint64)));
    connect(ui->volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(setVolume(int)));
    connect(ui->volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(volumeChangedSlot(int)));
    connect(ui->playSoundButton, SIGNAL(clicked()), this, SLOT(playButtonPressedSlot()));
    connect(ui->stopSoundButton, SIGNAL(clicked()), this, SLOT(stopButtonPressedSlot()));

    if(!couldNotBeLoaded.isEmpty())
    {
        this->show();
        QMessageBox::warning(this, tr("The following files could not be loaded"), couldNotBeLoaded, QMessageBox::Ok);
    }
}

SoundSelector::~SoundSelector()
{
    delete ui;
    delete mediaPlayer;
}

void SoundSelector::switchToAnotherParent(QTreeWidgetItem *item, int index)
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

void SoundSelector::setNewMedia(const QString &path_to_media)
{
    // set new media for player
    mediaPlayer->setMedia(QUrl::fromLocalFile(QString(path_to_media)));

    ui->durationLabel->setText(QString("Duration: %1 sec").arg((qreal)(mediaPlayer->duration())/1000.0));

    QFileInfo fileToPlay(path_to_media);
    enablePlayer(fileToPlay.fileName());
}

void SoundSelector::keyPressEvent(QKeyEvent *e)
{
    if(e->modifiers()==Qt::ControlModifier)
    {
        // if CTRL+B is pressed try to link selected items
        if(e->key()==Qt::Key_B)
            linkSelectedItem();
        // if CTRL+N is pressed try to unlink selected items
        else if(e->key()==Qt::Key_N)
            unlinkSelectedItems();
        // if CTRL+X is pressed try to remove selected sounds items
        else if(e->key()==Qt::Key_X)
            deleteSelectedSounds();
    }

    QDialog::keyPressEvent(e);
}

void SoundSelector::linkSelectedItem()
{
    if(!ui->rolesTree->selectedItems().isEmpty() && !ui->importedSoundsList->selectedItems().isEmpty())
    {
        if(ui->rolesTree->selectedItems().size()>1 || ui->importedSoundsList->selectedItems().size()>1)
            QMessageBox::warning(this, tr("Too many items"), tr("Select only one item in roles tree and sounds list to link."),
                                              QMessageBox::Ok);
        else
        {
            // do nothing if top level item is selected, or item in linked category is highlighted
            if(ui->rolesTree->selectedItems().first()->parent()==NULL ||
                    ui->rolesTree->selectedItems().first()->parent()==ui->rolesTree->topLevelItem(0)) return;

            // get the sound path and set the new image to appropriate object
            QString path = ui->importedSoundsList->selectedItems().first()->data(Qt::UserRole).toString();
            int index = ui->rolesTree->selectedItems().first()->data(1, Qt::UserRole).toInt();

            // set new sound
            // soundItems->at(index)->setImage(path); // OLD CODE
            soundItems->at(index)->setTempSoundPath(path);

            // if incremental switching is allowed, we need to remember the index of selected item
            int curr_position_role = 0;
            int curr_position_item = 0; // sound item;
            if(ui->incrementalSwitchCheckbox->isChecked())
            {
                curr_position_role = ui->rolesTree->topLevelItem(1)->indexOfChild(ui->rolesTree->selectedItems().first());
                curr_position_item = ui->importedSoundsList->row(ui->importedSoundsList->selectedItems().first());
            }

            // migrate the item to linked category
            switchToAnotherParent(ui->rolesTree->selectedItems().first(), 0);

            // remove sound record in sounds list
            delete ui->importedSoundsList->selectedItems().first();

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

                // do the same logic for sound items list
                int sounds_count = ui->importedSoundsList->count();

                // no more items to link in sound list
                if(sounds_count!=0)
                {
                    if(sounds_count==curr_position_item)
                        curr_position_item = 0; // we reached end of list, returning to the first item

                    ui->importedSoundsList->item(curr_position_item)->setSelected(true);
                }
            }
        }
    }
    else QMessageBox::warning(this, tr("Invalid selection"), tr("There is no role or sound selected for link operation."),
                              QMessageBox::Ok);
}

void SoundSelector::unlinkSelectedItems()
{
    if(!ui->rolesTree->selectedItems().isEmpty())
    {
        foreach(QTreeWidgetItem * item, ui->rolesTree->selectedItems())
        {
            // do nothing if top level item is selected, or item in unlinked category is highlighted
            if(item->parent()==NULL || item->parent()==ui->rolesTree->topLevelItem(1)) continue;

            // discard initial sound, so object is empty again
            int index = item->data(1, Qt::UserRole).toInt();
            QString snd_path = soundItems->at(index)->getTempSoundPath();

            soundItems->at(index)->clearTempSoundPath();
            // soundItems->at(index)->discardImage(); // OLD CODE

            // if incremental switching is allowed, we need to remember the index of selected item
            int curr_position_role = 0;
            if(ui->incrementalSwitchCheckbox->isChecked())
            {
                curr_position_role = ui->rolesTree->topLevelItem(0)->indexOfChild(ui->rolesTree->selectedItems().first());
            }

            // migrate the item to unlinked category
            switchToAnotherParent(item, 1);

            // add released sound back to sounds list
            QFileInfo curr_file(snd_path);
            if(curr_file.exists())
            {
                // if sound still exists, we can return the record about it back to the sounds list
                QListWidgetItem * item = new QListWidgetItem(curr_file.fileName());
                item->setData(Qt::UserRole, snd_path); // save path to the sound as data
                ui->importedSoundsList->addItem(item); // create new item in list
            }
            else QMessageBox::warning(this, tr("Sound cannot be found"), tr("During attempt to return sound back into "
                                                                            "file list program found, that sound is not "
                                                                            "longer at given location. Sound was discarded."),
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

void SoundSelector::loadSelectedSounds()
{
    QStringList selectedFiles = QFileDialog::getOpenFileNames(this, tr("Select sounds to load"), QDir::currentPath(),
                                                                QString("Image files (*.wav)"));
    // if user clicks cancel, or selects no items
    if(selectedFiles.isEmpty()) return;

    // create items in list widgets
    foreach(QString file, selectedFiles)
    {
        QFileInfo curr_item(file);
        // add new widget to the list
        QListWidgetItem * item = new QListWidgetItem(curr_item.fileName());
        item->setData(Qt::UserRole, file); // save path to the sound as data
        ui->importedSoundsList->addItem(item);
        importedSounds->append(file);
    }
}

void SoundSelector::loadSoundsFromDir()
{
    QDir selected_dir;
    selected_dir.setPath(QFileDialog::getExistingDirectory(this, tr("Select folder with sounds"), QDir::currentPath(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));

    // if user clicks cancel button, returned string is empty
    if(selected_dir.path().isEmpty()) return;

    // Iterate over all files in directory, so we can check if they are sounds.
    // I decided to not recursively iterate into subdirectories.
    QDirIterator it(selected_dir.path(), QDir::Files, QDirIterator::NoIteratorFlags);
    while(it.hasNext())
    {
        // check if is sound in format wav
        QFileInfo file(it.next());
        if(file.suffix()=="wav")
        {
            // file is sound, so we can add it to the imported list
            QListWidgetItem * item = new QListWidgetItem(file.fileName());
            item->setData(Qt::UserRole, file.filePath()); // save path to the sounds as data
            ui->importedSoundsList->addItem(item);
            importedSounds->append(file.filePath());
        }
    }
}

void SoundSelector::deleteSelectedSounds()
{
    foreach(QListWidgetItem * selected, ui->importedSoundsList->selectedItems())
    {
        // every selected item in imported images list will be immediately removed from list and deleted
        delete selected;
    }
}

void SoundSelector::soundListWasClicked(void)
{
    // check if there is only one item selected
    if(ui->importedSoundsList->selectedItems().count()>1 || ui->importedSoundsList->selectedItems().isEmpty())
    {
        // cannot preview more items at once
        disablePlayer();
    }
    else
    {
        if(preview!=NULL)
        {
            delete preview;
            preview = NULL;
        }

        QString path = ui->importedSoundsList->selectedItems().first()->data(Qt::UserRole).toString();

        // If original_handler is not NULL, we need to set it up so - otherwise during slider interaction
        // slots will try also modify the original handler although, since we are here loading only unlinked
        // file, there is no such thing.
        original_handler = NULL;


        preview = new sound_handler(path, -1, -1, default_volume_level); // preview can be created also without role code or index
        if(!preview->isEmpty())
        {
            setNewMedia(path);
        }
        else
        {
            // reset preview and inform user about problem
            disablePlayer();

            QMessageBox::warning(this, tr("Sound could not be loaded"),
                                  tr("The selected sound could not be loaded. Check if the sound is still availible on "
                                     "your disk or is in correct format (wav)."), QMessageBox::Ok);
        }
    }
}

void SoundSelector::soundRoleWasClicked()
{
    // check if there is only one item selected
    if(ui->rolesTree->selectedItems().count()>1 || ui->rolesTree->selectedItems().isEmpty())
    {
        // cannot preview more items at once - display informative text
        disablePlayer();
    }
    else
    {
        // if the selected item is not linked item, or it is top level item, do nothing
        if(ui->rolesTree->selectedItems().first()->parent()==NULL ||
           ui->rolesTree->selectedItems().first()->parent()==ui->rolesTree->topLevelItem(1))
        {
            disablePlayer();
            return;
        }

        int index = ui->rolesTree->selectedItems().first()->data(1, Qt::UserRole).toInt();

        if(preview!=NULL)
        {
            delete preview;
            preview = NULL;
        }

        original_handler = soundItems->at(index);
        QString path = soundItems->at(index)->getTempSoundPath();
        int volume = soundItems->at(index)->getVolumeLevel();
        if(path.isEmpty())
        {
            disablePlayer();
            return;
        }

        preview = new sound_handler(path, -1, -1, volume); // preview can be displayed also without role code or index

        // if sound is not correctly loaded, it may be useful to inform the user about error
        if(!preview->isEmpty())
        {
            setNewMedia(path);
        }
        else
        {
            // reset preview and inform user about problem
            disablePlayer();

            QMessageBox::warning(this, tr("Image could not be loaded"),
                                  tr("The selected image could not be loaded. Check if the image is still availible on "
                                     "your disk or is in correct format (bmp, png, jpg)."), QMessageBox::Ok);
        }
    }
}

void SoundSelector::cancelButtonClicked()
{
    // if user clicked cancel button, we must discard temporary paths
    for(QList<sound_handler * >::iterator it=soundItems->begin(); it!=soundItems->end(); it++)
        (*it)->clearTempSoundPath();
}

void SoundSelector::acceptButtonClicked()
{
    QString result;

    // append new items to original list
    for(QList<sound_handler * >::iterator it=soundItems->begin(); it!=soundItems->end(); it++)
    {
        // check if there is some sound to be loaded
        if((*it)->getTempSoundPath().isEmpty())
        {
            (*it)->discardSound(); // delete sound, if there is some
            continue;
        }

        int loading_result = (*it)->setSoundPath((*it)->getTempSoundPath());
        if(loading_result!=1) // an error occured when loading sound
        {
            result.append(QString("The file: \"%1\" could not be loaded").arg((*it)->getTempSoundPath()));
        }
    }

    // clear importedSounds vector, we will fill it with new values
    importedSounds->clear();
    for(int i=0; i<ui->importedSoundsList->count(); i++)
    {
        QListWidgetItem * temp = ui->importedSoundsList->item(i);
        importedSounds->append(temp->data(Qt::UserRole).toString());
    }

    // if there is some error message to be displayed
    if(!result.isEmpty()) QMessageBox::warning(this, tr("Some sounds could not be loaded"), result, QMessageBox::Ok);

    *ok_s = true;
}

void SoundSelector::volumeChangedSlot(int new_pos)
{
    if(original_handler!=NULL)
    {
        // change the volume for specific sound if is selected
        original_handler->setVolumeLevel(new_pos);
    }
    else
    {
        // change the volume of global volume level used for imported files
        default_volume_level = new_pos;
    }

    ui->volumeLabel->setText(QString("Volume (%1):").arg(new_pos));
}

void SoundSelector::setVolume(int new_vol)
{
    mediaPlayer->setVolume(new_vol*10);
}

void SoundSelector::progressChangedSlot(qint64 new_pos)
{
    if(ui->progressSlider->maximum()==new_pos)
    {
        // media playing is finished
        ui->stopSoundButton->setDisabled(true);
        ui->playSoundButton->setEnabled(true);
        ui->progressSlider->setValue(0);
    }
    else ui->progressSlider->setValue(new_pos);
}

void SoundSelector::progressUserChangedSlot(int new_pos)
{
    if(ui->progressSlider->maximum()==new_pos)
    {
        // media playing is finished
        ui->stopSoundButton->setDisabled(true);
        ui->playSoundButton->setEnabled(true);
        ui->progressSlider->setValue(0);
        mediaPlayer->stop(); // if music is still playing
    }
    else mediaPlayer->setPosition((qint64)(new_pos));
}

void SoundSelector::durationChangedSlot(qint64 new_maximum)
{
    ui->progressSlider->setMaximum(new_maximum);
    ui->durationLabel->setText(QString("Duration: %1 sec").arg((qreal)(mediaPlayer->duration())/1000.0));
}

void SoundSelector::playButtonPressedSlot()
{
    ui->stopSoundButton->setEnabled(true);
    ui->playSoundButton->setDisabled(true);
    mediaPlayer->play();
}

void SoundSelector::stopButtonPressedSlot()
{
    ui->stopSoundButton->setDisabled(true);
    ui->playSoundButton->setEnabled(true);
    mediaPlayer->stop();
}

void SoundSelector::disablePlayer()
{
    ui->volumeLabel->setDisabled(true);
    ui->volumeSlider->setDisabled(true);
    ui->playSoundButton->setDisabled(true);
    ui->stopSoundButton->setDisabled(true);
    ui->progressSlider->setDisabled(true);
    ui->progressLabel->setDisabled(true);
    ui->durationLabel->setVisible(false);

    mediaPlayer->stop();

    ui->fileToPlayLabel->setText(tr("no file to play"));
    original_handler = NULL;
}

void SoundSelector::enablePlayer(QString filename)
{
    ui->volumeLabel->setDisabled(false);
    ui->volumeSlider->setDisabled(false);
    ui->playSoundButton->setDisabled(false);
    ui->stopSoundButton->setDisabled(true);
    ui->progressSlider->setDisabled(false);
    ui->progressLabel->setDisabled(false);
    ui->durationLabel->setVisible(true);

    ui->volumeSlider->setValue(preview->getVolumeLevel());
    ui->progressSlider->setValue(0);

    QString volume_str = tr("Volume (");
    volume_str.append(QString("%1):").arg(preview->getVolumeLevel()));
    ui->volumeLabel->setText(volume_str);

    if(filename.length()>15)
    {
        QFileInfo f(filename);
        filename.remove(15, filename.length()-15);
        filename.append(QString("...")+f.suffix());
    }
    ui->fileToPlayLabel->setText(filename);
}
