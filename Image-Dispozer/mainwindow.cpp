#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    item_opaque_animation_delay(500), border_resize_animation_delay(500), border_pen_width(4.0)
{
    ui->setupUi(this);

    // let tempData to be NULL at startup until first save request by user
    tempItemData = lastItemDataUpdate = NULL;

    // this value should be set to true immediately if something in project is changed
    something_changed = false;

    initializeTreeItems();

    // create new scene and view
    scene = new imageScene(this);
    scene->setBackgroundBrush(QBrush(QColor(36, 38, 41), Qt::SolidPattern));
    image_mode = SINGLE;
    view = new imageView(scene, this);
    view->setOrthogonalRotation(false);
    // add border to the scene
    ratio_mode = LANDSCAPE;
    resize_on_ratio_change = reposition_on_ratio_change = true;
    hSize = 320;
    vSize = 240;

    // here initial scale is being calculated/set
    qreal init_scale = 1.8;

    // zoom in a little, so the initial view is bigger and better to be seen
    view->scale(init_scale, -init_scale);
    view->setCurrentScaleFactor(1.8);
    offset = 6.0; // because of borders (when windows gets too small and scrollbars are to be appeared)
    view->setSceneRect(-offset, -offset, hSize+offset*2.0, vSize+offset*2.0); // +-offset is because of scrollbars which appear if windows is too small
    view->centerOn(hSize/2.0, vSize/2.0);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setInteractive(true);
    view->setMouseTracking(true);

    connect(view, SIGNAL(updateDisplayedItemsVector(resizeRect*)), this, SLOT(updateDisplayedItemsVector(resizeRect*)));
    connect(view, SIGNAL(saveSelectedItemData(image_handler*)), this, SLOT(saveSelectedItemData(image_handler*)));
    connect(view, SIGNAL(currentSingleItemSelection(image_handler*)), this, SLOT(updateElementInfo(image_handler*)));

    // add border
    borderRectangle = new borderRect(0.0-border_pen_width/2.0, 0.0-border_pen_width/2.0, hSize+border_pen_width, vSize+border_pen_width, NULL);
    borderRectangle->setPen(QPen(QBrush(QColor(80, 80, 80)), border_pen_width, Qt::SolidLine, Qt::SquareCap));
    borderRectangle->setBrush(QBrush(QColor(220, 220, 220)));
    scene->addItem(borderRectangle);

    // display view in mainWindow
    QVBoxLayout * sceneWidgetLayout = new QVBoxLayout;
    sceneWidgetLayout->setContentsMargins(0, 0, 0, 0);
    sceneWidgetLayout->addWidget(view);
    ui->sceneWidget->setLayout(sceneWidgetLayout);

    // set window size to try show up all rectangle
    if(hSize*init_scale*0.6<1336 && vSize*init_scale*0.6<768)
        this->setGeometry(100, 100, hSize*init_scale+150.0+ui->rolesListWidget->width(), vSize*init_scale+150.0);


    // set wrapper of quick buttons with no margins
    ui->quickButtonsWrapperWidget->layout()->setContentsMargins(0, 0, 0, 0);

    // TEST PURPOSES ONLY
    /*resizeRect * r_rect = new resizeRect(hSize/2.0, vSize/2.0, 100, 100, NULL);
    image_handler * i_handler =  new image_handler(QString("C:\\Users\\PeterMikula\\Desktop\\DISPLAY_APP\\SD_CONTENT\\BMP\\pic_001.bmp"), 0, 0);
    r_rect->setPixmap(i_handler);
    scene->addItem(r_rect);*/

    temp_ini_file_path.clear();

    period = 4;
    ack_banel_error = false;
    volume = -12;

    connect(ui->actionImport, SIGNAL(triggered()), this, SLOT(imageSelectorWindow()));
    connect(ui->actionOpen_profile, SIGNAL(triggered()), this, SLOT(initFileLoaderWindow()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveProfileSlot()));
    connect(ui->actionSave_As, SIGNAL(triggered()), this, SLOT(saveAsProfileSlot()));
    connect(ui->actionContact_data, SIGNAL(triggered()), this, SLOT(contactDataWindow()));
    connect(ui->actionMain_CFG, SIGNAL(triggered()), this, SLOT(mainCfgWindow()));

    connect(ui->rolesListWidget, SIGNAL(currentRowChanged(int)), SLOT(displayNewRectItem(int)));
    connect(ui->clearSceneItemsButton, SIGNAL(clicked()), this, SLOT(removeAllDisplayedItems()));
    connect(ui->switchPortraitLandscapeButton, SIGNAL(clicked()), this, SLOT(togglePortraitLandscapeMode()));
    connect(ui->switchSingleMultipleImagesButton, SIGNAL(clicked()), this, SLOT(toggleSingleMultipleImageMode()));
    connect(ui->applySavedDataButton, SIGNAL(clicked()), this, SLOT(applySavedDataSlot()));

    connect(ui->rotationDataLabel, SIGNAL(triggered()), this, SLOT(updateItemRotation()));
    connect(ui->sizeDataLabel, SIGNAL(triggered()), this, SLOT(updateItemSize()));
    connect(ui->positionDataLabel, SIGNAL(triggered()), this, SLOT(updateItemPosition()));
    connect(ui->lbDataLabel, SIGNAL(triggered()), this, SLOT(updateItemLBCorner()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeTreeItems()
{
    rolesList = new QVector<roleString * >;
    imageItems = new QList<image_handler * >;
    importedImages = new QStringList;

    // fill the rolesList with relevant items
    // NOTE that roleList is not allowed to be modified elsewhere in the program.
    // If more items are needed, they are supposed to be added HERE.
    rolesList->append(new roleString("Element \" 0\"", 0));
    rolesList->append(new roleString("Element \" 1\"", 1));
    rolesList->append(new roleString("Element \" 2\"", 2));
    rolesList->append(new roleString("Element \" 3\"", 3));
    rolesList->append(new roleString("Element \" 4\"", 4));
    rolesList->append(new roleString("Element \" 5\"", 5));
    rolesList->append(new roleString("Element \" 6\"", 6));
    rolesList->append(new roleString("Element \" 7\"", 7));
    rolesList->append(new roleString("Element \" 8\"", 8));
    rolesList->append(new roleString("Element \" 9\"", 9));
    rolesList->append(new roleString("Element \"10\"", 10));
    rolesList->append(new roleString("Element \"11\"", 11));
    rolesList->append(new roleString("Element \"12\"", 12));
    rolesList->append(new roleString("Element \"13\"", 13));
    rolesList->append(new roleString("Element \"14\"", 14));
    rolesList->append(new roleString("Element \"15\"", 15));
    rolesList->append(new roleString("Element \"16\"", 16));
    rolesList->append(new roleString("Element \"17\"", 17));
    rolesList->append(new roleString("Element \"18\"", 18));
    rolesList->append(new roleString("Element \"19\"", 19));
    rolesList->append(new roleString("Element \"20\"", 20));

    rolesList->append(new roleString("Element \"-1\"", 21));
    rolesList->append(new roleString("Element \"-2\"", 22));
    rolesList->append(new roleString("Element \"-3\"", 23));
    rolesList->append(new roleString("Element \"-4\"", 24));
    rolesList->append(new roleString("Element \"-5\"", 25));
    rolesList->append(new roleString("Element \"-6\"", 26));
    rolesList->append(new roleString("Element \"-7\"", 27));
    rolesList->append(new roleString("Element \"-8\"", 28));
    rolesList->append(new roleString("Element \"-9\"", 29));

    rolesList->append(new roleString("Element \"(empty)\"", 40));
    rolesList->append(new roleString("Element \" A\"", 41));
    rolesList->append(new roleString("Element \"A1\"", 42));
    rolesList->append(new roleString("Element \" B\"", 43));
    rolesList->append(new roleString("Element \" E\"", 44));
    rolesList->append(new roleString("Element \"E1\"", 45));
    rolesList->append(new roleString("Element \" G\"", 46));
    rolesList->append(new roleString("Element \"G1\"", 47));
    rolesList->append(new roleString("Element \" F\"", 48));
    rolesList->append(new roleString("Element \" N\"", 49));
    rolesList->append(new roleString("Element \"SA\"", 50));
    rolesList->append(new roleString("Element \"SS\"", 51));
    rolesList->append(new roleString("Element \"S1\"", 52));
    rolesList->append(new roleString("Element \" R\"", 53));
    rolesList->append(new roleString("Element \" P\"", 54));
    rolesList->append(new roleString("Element \" H\"", 55));
    rolesList->append(new roleString("Element \"--\"", 56));

    rolesList->append(new roleString("Element \"RV\"", 62));

    // now all items are unlinked, add each item into unlinkedItems vector
    for(int i = 0; i<rolesList->size(); i++)
    {
        imageItems->append(new image_handler(QString(), rolesList->at(i)->getRoleCode(), i));
        imageItems->last()->setPosition(QPointF(100.0, 100.0));
    }

    updateRolesListWidget();

    displayedItems = new QList<resizeRect * >;
}

void MainWindow::updateRolesListWidget()
{
    ui->rolesListWidget->clear();

    int index, i;
    i=0;
    for(QList<image_handler * >::iterator it = imageItems->begin(); it<imageItems->end(); it++)
    {
        index = (*it)->getIndex();
        QListWidgetItem * item = new QListWidgetItem(*rolesList->at(index));
        item->setData(Qt::UserRole, i); // each item will have direct access to image_handler
        ui->rolesListWidget->addItem(item);
        ++i;
    }

    // update color of items, so user will clearly see which items are already connected
    updateRolesListWidgetColor();
}

void MainWindow::updateRolesListWidgetColor()
{
    int index = 0;
    QList<QListWidgetItem * > tempItemList =  ui->rolesListWidget->findItems(QString("*"), Qt::MatchWildcard);
    for(QList<QListWidgetItem * >::iterator it = tempItemList.begin(); it<tempItemList.end(); it++)
    {
        index = (*it)->data(Qt::UserRole).toInt();
        if(imageItems->at(index)->isFileCorrect())
            (*it)->setTextColor(Qt::darkGreen); // if item is associated
        else
            (*it)->setTextColor(Qt::red); // if no image is associated
    }
}

void MainWindow::imageSelectorWindow()
{
    // create dialog window for selecting and linking images to their appropriate roles
    bool ok;
    ImageSelector im_select_window(rolesList, imageItems, importedImages, &ok, this);
    im_select_window.exec();

    updateRolesListWidgetColor();

    this->centralWidget()->setFocusPolicy(Qt::StrongFocus);

    // if user did not clicked cancel, we will believe that something was updated
    if(ok) somethingChangedSlot();
}

void MainWindow::displayNewRectItem(int row)
{
    Q_UNUSED(row)

    int index = ui->rolesListWidget->currentItem()->data(Qt::UserRole).toInt();

    if(!imageItems->at(index)->isCurrentlyDisplayed())
    {
        if(imageItems->at(index)->isFileCorrect())
        {
            QSizeF size = imageItems->at(index)->getItemSize();
            QPointF pos = imageItems->at(index)->getPosition();
            qreal rot   = imageItems->at(index)->getItemRotation();

            // note that you can access xSize or ySize from inside the
            resizeRect * r_rect = new resizeRect(pos.x(), pos.y(), size.width(), size.height(), NULL);
            r_rect->setRotation(rot);
            // set item handler status of currently displayed boolean to true
            imageItems->at(index)->setCurrentlyDisplayed(true);
            r_rect->setPixmap(imageItems->at(index));

            // check for mode, if SINGLE mode is enabled, we will clear other images in scene before adding new
            if(image_mode==SINGLE) removeAllDisplayedItems();

            displayedItems->append(r_rect);

            // append to rect items vector, so we have easy access to all rect items in scene
            scene->addItem(r_rect);
        }
        else ui->statusBar->showMessage(tr("This element has not been associated yet"));
    }
    else ui->statusBar->showMessage(tr("This element is already displayed"));
}

void MainWindow::updateDisplayedItemsVector(resizeRect * item)
{
    // do not delete object itself, since it is done automatically by view
    for(int i=0; i<displayedItems->count(); i++)
        if(item==displayedItems->at(i)) displayedItems->removeAt(i);
}

void MainWindow::togglePortraitLandscapeMode(bool just_update)
{
    if(ratio_mode==LANDSCAPE)
    {
        // if just update is false, we will also switch the mode, else this function will behave like update to currently set mode
        if(!just_update) ratio_mode = PORTRAIT;
        ui->switchPortraitLandscapeButton->setIcon(QIcon(":/quick_access/icons/landscape-icon.png"));
    }
    else if(ratio_mode==PORTRAIT)
    {
        // if just update is false, we will also switch the mode, else this function will behave like update to currently set mode
        if(!just_update) ratio_mode = LANDSCAPE;
        ui->switchPortraitLandscapeButton->setIcon(QIcon(":/quick_access/icons/portrait-icon.png"));
    }

    if(!just_update)
    {
        qreal temp = hSize;
        hSize = vSize;
        vSize = temp;
    }

    hideDisplayedItems();
    //QThread::msleep(item_opaque_animation_delay);

    // resize the border rect
    QPropertyAnimation * border_animation = new QPropertyAnimation(borderRectangle, "size");
    border_animation->setDuration(border_resize_animation_delay);
    border_animation->setStartValue(borderRectangle->getSize());
    border_animation->setEndValue(QSizeF(hSize+border_pen_width, vSize+border_pen_width));

    // animate resizing of scene rect, so the rectangle will not visualy just jump to another position like
    QPropertyAnimation * scene_rect_animation = new QPropertyAnimation(view, "sceneRect");
    scene_rect_animation->setDuration(border_resize_animation_delay);
    scene_rect_animation->setStartValue(view->sceneRect());
    scene_rect_animation->setEndValue(QRectF(-offset, -offset, hSize+border_pen_width+offset*2.0, vSize+border_pen_width+offset*2.0));

    scene_rect_animation->start(QAbstractAnimation::DeleteWhenStopped);
    border_animation->start(QAbstractAnimation::DeleteWhenStopped);

    // after animation is finished, show previous items again
    connect(border_animation, SIGNAL(finished()), this, SLOT(revealDisplayedItems()));

    if(resize_on_ratio_change || reposition_on_ratio_change)
    {
        qreal ratio = hSize/vSize;
        for(QList<image_handler * >::iterator it=imageItems->begin(); it!=imageItems->end(); it++)
        {
            // udate positions of objects to fit in new dimensions
            if(reposition_on_ratio_change)
            {
                QPointF previous = (*it)->getPosition();
                (*it)->setPosition(QPointF(previous.x()*ratio, previous.y()/ratio));
            }

            // update sizes of objects if this option is enabled
            if(resize_on_ratio_change)
            {
                QSizeF previous = (*it)->getItemSize();
                (*it)->setItemSize(QSizeF(previous.width()*ratio, previous.height()/ratio));
            }
        }
    }

    // update displayed items (it there are any)
    updateVisibleItems();

    somethingChangedSlot();

    scene->update();
}

void MainWindow::toggleSingleMultipleImageMode(bool just_update)
{
    if(image_mode==SINGLE)
    {
        // if just update is false, we will also switch the mode, else this function will behave like update to currently set mode
        if(!just_update) image_mode = MULTIPLE;
        ui->switchSingleMultipleImagesButton->setIcon(QIcon(":/quick_access/icons/single-image-icon.png"));
    }
    else if(image_mode==MULTIPLE)
    {
        ui->switchSingleMultipleImagesButton->setIcon(QIcon(":/quick_access/icons/multiple-images-icon.png"));
        // if just update is false, we will also switch the mode, else this function will behave like update to currently set mode
        if(!just_update) image_mode = SINGLE;

        // if going to single mode, we need to delete all objects in scene currently visible
        removeAllDisplayedItems();
    }
}

void MainWindow::saveSelectedItemData(image_handler *data)
{
    if(tempItemData==NULL) tempItemData = new image_handler();

    tempItemData->setPosition(data->getPosition());
    tempItemData->setItemSize(data->getItemSize());
    tempItemData->setItemRotation(data->getItemRotation());
}

void MainWindow::applySavedDataSlot()
{
    if(!ui->rolesListWidget->selectedItems().isEmpty() && tempItemData!=NULL)
    {
        // apply saved data on all selected items
        Q_FOREACH(QListWidgetItem * item, ui->rolesListWidget->selectedItems())
        {
            int index = item->data(Qt::UserRole).toInt(); // data specifies index of image handler in its list
            // update data of item
            imageItems->at(index)->setPosition(tempItemData->getPosition());
            imageItems->at(index)->setItemSize(tempItemData->getItemSize());
            imageItems->at(index)->setItemRotation(tempItemData->getItemRotation());
        }

        somethingChangedSlot();
    }

    view->viewport()->update();
}

void MainWindow::hideDisplayedItems()
{
    for(int i=0; i<displayedItems->count(); i++)
    {
        QPropertyAnimation * item_animation = new QPropertyAnimation(displayedItems->at(i), "opaque");
        item_animation->setDuration(item_opaque_animation_delay);
        item_animation->setStartValue(1.0);
        item_animation->setEndValue(0.0);
        item_animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void MainWindow::revealDisplayedItems()
{
    for(int i=0; i<displayedItems->count(); i++)
    {
        QPropertyAnimation * item_animation = new QPropertyAnimation(displayedItems->at(i), "opaque");
        item_animation->setDuration(item_opaque_animation_delay);
        item_animation->setStartValue(0.0);
        item_animation->setEndValue(1.0);
        item_animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void MainWindow::updateElementInfo(image_handler *item)
{
    // if item contains some pointer, we will extract data
    if(item!=NULL)
    {
        QPointF pos = item->getPosition();
        QPointF lb = item->getLBCorner();
        QSizeF siz = item->getItemSize();
        qreal rotation = item->getItemRotation();
        int index = item->getIndex(); // we suppose here, that role index is the same as its role index

        ui->nameDataLabel->setText(*rolesList->at(index));

        // if using float numbers format, use additional parameters for arg(xxx, 5, 'f', 2, '0')

        ui->positionDataLabel->setText(QString("[%1, %2]").arg((int)(pos.x())).arg((int)(pos.y())));
        ui->lbDataLabel->setText(QString("[%1, %2]").arg((int)(lb.x())).arg((int)(lb.y())));
        ui->rotationDataLabel->setText(QString("%1°").arg(rotation));
        ui->sizeDataLabel->setText(QString("%1 %2").arg((int)(siz.width())).arg((int)(siz.height())));

        // prevents string/path parsing too often
        if(item!=lastItemDataUpdate)
        {
            QFileInfo f_info(item->getImagePath());

            if(!f_info.exists())
            {
                ui->imageDataLabel->setText(tr("not found")); // if image is not present on specified path (inform the user)
                ui->imageDataLabel->setToolTip(tr("No image was found"));
            }
            else
            {
                ui->imageDataLabel->setText(f_info.fileName());
                ui->imageDataLabel->setToolTip(QString("%1 : %2").arg(f_info.fileName()).arg(f_info.filePath()));
            }
        }
    }
    else
    {
        // set default values for each label
        ui->nameDataLabel->setText("xxx");
        ui->positionDataLabel->setText("xxx");
        ui->lbDataLabel->setText("xxx");
        ui->rotationDataLabel->setText("xxx");
        ui->sizeDataLabel->setText("xxx");
        ui->imageDataLabel->setText("xxx");
        ui->imageDataLabel->setToolTip(tr("No image availible"));
    }

    // Update last updated pointer, so some operations above does not have to run at next iteration.
    // For example during mouse move, QFileInfo operations can really slow down the process.
    lastItemDataUpdate = item;
}

void MainWindow::updateVisibleItems()
{
    // if data in image_handlers is changed, we need to call update function of each visible item, to immediately apply changes
    for(int i=0; i<displayedItems->count(); i++)
        displayedItems->at(i)->updateData();
}

void MainWindow::saveAsProfileSlot()
{
    // we will automatically clear temp_ini_file_path here, so saveProfileSlot() will surely ask for new path
    temp_ini_file_path.clear();
    saveProfileSlot();
}

void MainWindow::updateItemRotation()
{
    resizeRect * rect = NULL;
    image_handler * item = view->checkForSingleSelection(false, false, &rect);

    if(item==NULL || rect==NULL) return; // if from some reasons no selected item could be found

    qreal angle = QInputDialog::getInt(this, tr("Angle"), tr("Set new angle"), item->getItemRotation(), 0, 359);

    // update infoLabel
    ui->rotationDataLabel->setText(QString("%1").arg(angle));

    // update item in scene and image handler
    item->setItemRotation(angle);
    rect->updateData();
}

void MainWindow::updateItemSize()
{
    resizeRect * rect = NULL;
    image_handler * item = view->checkForSingleSelection(false, false, &rect);

    if(item==NULL || rect==NULL) return; // if from some reasons no selected item could be found

    // set list with labels
    QStringList labels;
    labels.append(tr("Image width:"));
    labels.append("Image height:");

    // set values list according to labels
    QList<int> values;
    values.append(item->getItemSize().width());
    values.append(item->getItemSize().height());

    // set intervals and increment step in order (min, max, step) for each value in list
    QList<int> intervals_step;
    intervals_step.append(5);
    intervals_step.append(500);
    intervals_step.append(1);
    intervals_step.append(5);
    intervals_step.append(500);
    intervals_step.append(1);

    bool ok;

    // start user input dialog
    manualDataInput * input = new manualDataInput(tr("Change size"), labels, &values, intervals_step, &ok, this);
    input->exec();

    // if ok is set to false, no change is going to be done, Cancel button was clicked
    if(!ok) return;

    // update infoLabel
    ui->sizeDataLabel->setText(QString("%1 %2").arg(values.at(0)).arg(values.at(1)));

    item->setItemSize(QSizeF(values.at(0), values.at(1)));
    rect->updateData();
}

void MainWindow::updateItemPosition()
{
    resizeRect * rect = NULL;
    image_handler * item = view->checkForSingleSelection(false, false, &rect);

    if(item==NULL || rect==NULL) return; // if from some reasons no selected item could be found

    // set list with labels
    QStringList labels;
    labels.append(tr("Image X:"));
    labels.append("Image Y:");

    // set values list according to labels
    QList<int> values;
    values.append(item->getPosition().x());
    values.append(item->getPosition().y());

    // set intervals and increment step in order (min, max, step) for each value in list
    QList<int> intervals_step;
    intervals_step.append(0);
    intervals_step.append(500);
    intervals_step.append(1);
    intervals_step.append(0);
    intervals_step.append(500);
    intervals_step.append(1);

    bool ok;

    // start user input dialog
    manualDataInput * input = new manualDataInput(tr("Change position"), labels, &values, intervals_step, &ok, this);
    input->exec();

    // if ok is set to false, no change is going to be done, Cancel button was clicked
    if(!ok) return;

    // update infoLabel
    ui->positionDataLabel->setText(QString("%1 %2").arg(values.at(0)).arg(values.at(1)));

    item->setPosition(QPointF(values.at(0), values.at(1)));
    rect->updateData();
}

void MainWindow::updateItemLBCorner()
{
    resizeRect * rect = NULL;
    image_handler * item = view->checkForSingleSelection(false, false, &rect);

    if(item==NULL || rect==NULL) return; // if from some reasons no selected item could be found

    // set list with labels
    QStringList labels;
    labels.append(tr("Image corner X:"));
    labels.append("Image corner Y:");

    // set values list according to labels
    QList<int> values;
    values.append(item->getLBCorner().x());
    values.append(item->getLBCorner().y());

    // set intervals and increment step in order (min, max, step) for each value in list
    QList<int> intervals_step;
    intervals_step.append(0);
    intervals_step.append(500);
    intervals_step.append(1);
    intervals_step.append(0);
    intervals_step.append(500);
    intervals_step.append(1);

    bool ok;

    // start user input dialog
    manualDataInput * input = new manualDataInput(tr("Change position"), labels, &values, intervals_step, &ok, this);
    input->exec();

    // if ok is set to false, no change is going to be done, Cancel button was clicked
    if(!ok) return;

    // update infoLabel
    ui->lbDataLabel->setText(QString("%1 %2").arg(values.at(0)).arg(values.at(1)));

    item->setLBCorner(QPointF(values.at(0), values.at(1)));
    // transform LB corner to position (LB corner behaves like information holder, not defining position of real item)
    item->setPosition(QPointF(values.at(0)+item->getItemSize().width()/2.0, values.at(1)+item->getItemSize().height()/2.0));
    rect->updateData();
}

void MainWindow::saveProfileSlot()
{
    // save settings into ini file
    QString file_to_save; // if we have some ini file loaded, we will try use that one first
    if(temp_ini_file_path.isEmpty())
    {
        file_to_save = QFileDialog::getSaveFileName(this, tr("Save profile"), QDir::currentPath(), tr("*.ini"));

        QFileInfo f_init(file_to_save);
        // We do not want to add suffix to file, which suffix already has. This can happen if
        // we choose to replace file in save file dialog. Also if user clicks cancel, we must not
        // to append anything, since later condition (detection of empty string) will fail
        // to detect user's cancel decision.
        if(!file_to_save.isEmpty() && f_init.suffix().isEmpty()) file_to_save.append(".ini");
    }
    else
    {
        // Check if file exists and really is .ini file. If not we will request for new file path and name.
        QFileInfo file_check(temp_ini_file_path);
        if(!file_check.exists() || file_check.suffix()!="ini")
        {
            file_to_save = QFileDialog::getSaveFileName(this, tr("Save profile"), QDir::currentPath(), tr("*.ini"));

            QFileInfo f_init(file_to_save);
            // We do not want to add suffix to file, which suffix already has. This can happen if
            // we choose to replace file in save file dialog. Also if user clicks cancel, we must not
            // to append anything, since later condition (detection of empty string) will fail
            // to detect user's cancel decision.
            if(!file_to_save.isEmpty() && f_init.suffix().isEmpty()) file_to_save.append(".ini");
        }
        else file_to_save = temp_ini_file_path;
    }

    if(!file_to_save.isEmpty())
    {
        temp_ini_file_path = file_to_save;

        QFileInfo f_info(file_to_save);

        QFile file_handler(file_to_save);
        // if file already exists, remove it
        if(f_info.exists()) file_handler.remove();

        QSettings setts(file_to_save, QSettings::IniFormat, this);

        // save scene setting
        setts.beginGroup("scene");
        setts.setValue("width", hSize);
        setts.setValue("height", vSize);
        setts.setValue("mode", ratio_mode);
        setts.setValue("im_mode", image_mode);
        setts.endGroup();

        // save configuration
        setts.beginGroup("config");
        setts.setValue("period", period);
        setts.setValue("perunits", per_units);
        setts.setValue("language", lan);
        setts.setValue("ackbanelerror", ack_banel_error);
        setts.setValue("volume", volume);
        setts.endGroup();

        // save all paths to all images
        setts.beginGroup("elements");
        int index;
        for(QList<image_handler * >::iterator it = imageItems->begin(); it!=imageItems->end(); it++)
        {
            index = (*it)->getIndex();
            QString result_key = rolesList->at(index)->replace(" ", "").replace("\"", "");
            setts.setValue(QString("%1_IMAGE_PATH").arg(result_key), (*it)->getImagePath().isEmpty() ? "none" : (*it)->getImagePath());
            setts.setValue(QString("%1_SIZE_W").arg(result_key), (*it)->getItemSize().width());
            setts.setValue(QString("%1_SIZE_H").arg(result_key), (*it)->getItemSize().height());
            setts.setValue(QString("%1_POSITION_X").arg(result_key), (*it)->getPosition().x());
            setts.setValue(QString("%1_POSITION_Y").arg(result_key), (*it)->getPosition().y());
            setts.setValue(QString("%1_BLCORNER_X").arg(result_key), (*it)->getLBCorner().x());
            setts.setValue(QString("%1_BLCORNER_Y").arg(result_key), (*it)->getLBCorner().y());
            setts.setValue(QString("%1_ROTATION").arg(result_key), (int)((*it)->getItemRotation()));
        }
        setts.endGroup();

        // save the contacts
        setts.beginGroup("contacts");
        setts.setValue("callnum1", call_number_1);
        setts.setValue("callnum2", call_number_2);
        setts.setValue("callnum3", call_number_3);
        setts.setValue("callnum4", call_number_4);
        setts.setValue("startsmsnumber", start_sms_number);
        setts.setValue("statsmsnumber", stat_sms_number);
        setts.setValue("alarm_smsnumber", alarm_sms_number);
        setts.setValue("protosmsnumber", proto_sms_number);
        setts.endGroup();

        something_changed = false;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton decision;

    decision = QMessageBox::question(this, tr("Closing application"), tr("Are you sure you want to quit?"), QMessageBox::Yes|QMessageBox::No);

    if(decision==QMessageBox::No)
    {
        // leave exit sequence
        event->ignore();
        return;
    }

    // if no change was done, do not ask for save
    if(!something_changed) return;

    // user really wish to quit
    decision = QMessageBox::question(this, tr("Save profile"), tr("Would you like to save your profile before exit?"), QMessageBox::Yes|QMessageBox::No);

    if(decision==QMessageBox::Yes)
        saveProfileSlot();
}

void MainWindow::initFileLoaderWindow()
{
    // load init file
    QString ini_file_path = QFileDialog::getOpenFileName(this, tr("Load profile"), QDir::currentPath(), tr("*.ini"));

    // no file selected
    if(ini_file_path.isEmpty()) return;

    QSettings setts(ini_file_path, QSettings::IniFormat, this);

    // store the ini file path so we can use for quick save actions
    temp_ini_file_path = ini_file_path;

    // READING SCENE SECTION OF DATA
    QString section = "scene";

    hSize = setts.value(QString("%1/width").arg(section), 320.0).toDouble();
    vSize = setts.value(QString("%1/height").arg(section), 240.0).toDouble();
    ratio_mode = (port_land_mode)(setts.value(QString("%1/mode").arg(section), LANDSCAPE).toInt());
    image_mode = (multiple_image_mode)(setts.value(QString("%1/im_mode").arg(section), SINGLE).toInt());

    // udpate img and ration modes without switching
    togglePortraitLandscapeMode(true);
    toggleSingleMultipleImageMode(true);


    // load contacts
    section.clear();
    section.append("contacts");

    start_sms_number = setts.value(QString("%1/startsmsnumber").arg(section), QString("")).toString();
    stat_sms_number = setts.value(QString("%1/statsmsnumber").arg(section), QString("")).toString();
    alarm_sms_number = setts.value(QString("%1/alarmsmsnumber").arg(section), QString("")).toString();
    proto_sms_number = setts.value(QString("%1/protosmsnumber").arg(section), QString("")).toString();
    call_number_1 = setts.value(QString("%1/callnum1").arg(section), QString("")).toString();
    call_number_2 = setts.value(QString("%1/callnum2").arg(section), QString("")).toString();
    call_number_3 = setts.value(QString("%1/callnum3").arg(section), QString("")).toString();
    call_number_4 = setts.value(QString("%1/callnum4").arg(section), QString("")).toString();

    // load config
    section.clear();
    section.append("config");

    period = setts.value(QString("%1/period").arg(section), 4).toInt();
    per_units = setts.value(QString("%1/perunits").arg(section), QString("hours")).toString();
    lan = setts.value(QString("%1/language").arg(section), QString("ENG")).toString();
    ack_banel_error = setts.value(QString("%1/ackbanelerror").arg(section), false).toBool();
    volume = setts.value(QString("%1/volume").arg(section), -12).toInt();

    // READING ELEMENTS SECTION OF DATA
    section.clear();
    section.append("elements");

    int index;
    for(QList<image_handler * >::iterator it = imageItems->begin(); it!=imageItems->end(); it++)
    {
        index = (*it)->getIndex();
        QString result_key = rolesList->at(index)->replace(" ", "").replace("\"", "");
        // generate result group/key string and load data
        QString img_path = setts.value(QString("%1/%2_IMAGE_PATH").arg(section).arg(result_key), "").toString();
        qreal w = setts.value(QString("%1/%2_SIZE_W").arg(section).arg(result_key), 100.0).toDouble();
        qreal h = setts.value(QString("%1/%2_SIZE_H").arg(section).arg(result_key), 100.0).toDouble();
        qreal x = setts.value(QString("%1/%2_POSITION_X").arg(section).arg(result_key), hSize/2.0).toDouble();
        qreal y = setts.value(QString("%1/%2_POSITION_Y").arg(section).arg(result_key), vSize/2.0).toDouble();
        qreal lbx = setts.value(QString("%1/%2_BLCORNER_X").arg(section).arg(result_key), hSize/2.0-100.0).toDouble();
        qreal lby = setts.value(QString("%1/%2_BLCORNER_Y").arg(section).arg(result_key), vSize/2.0-100.0).toDouble();
        qreal rot = setts.value(QString("%1/%2_ROTATION").arg(section).arg(result_key), 0.0).toDouble();

        // DO SOME CORRECTIONS OR CHECKS IF NEEDED
        if(img_path=="none") img_path.clear(); // if none statement is present, we will set empty string
        // %360 just in case that data will be corrupted and a random number occures (experience during testing, but hopefully repaired)
        rot = (int)(rot)%360;
        rot = rot>0.0 ? rot : -rot;

        // now update appropriate image handler with loaded values
        (*it)->setImage(img_path);
        (*it)->setItemSize(QSizeF(w, h));
        (*it)->setPosition(QPointF(x, y));
        (*it)->setLBCorner(QPointF(lbx, lby));
        (*it)->setItemRotation(rot);
    }

    // update items that have correctly loaded images
    updateRolesListWidgetColor();

    // we have new clear profile loaded
    something_changed = false;
}

void MainWindow::contactDataWindow()
{
    contactDataDialog dialog(&start_sms_number, &stat_sms_number, &alarm_sms_number, &proto_sms_number,
                             &call_number_1, &call_number_2, &call_number_3, &call_number_4, this);
    dialog.exec();
}

void MainWindow::mainCfgWindow()
{
    mainCFGDialog dialog(&period, &per_units, &lan, &ack_banel_error, &volume, this);
    dialog.exec();
}


void MainWindow::removeAllDisplayedItems()
{
    while(!displayedItems->isEmpty())
    {
        displayedItems->first()->deleteLater();
        displayedItems->removeFirst();
    }
}
