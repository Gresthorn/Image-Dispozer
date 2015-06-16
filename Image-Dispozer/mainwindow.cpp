#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    item_opaque_animation_delay(500), border_resize_animation_delay(500)
{
    ui->setupUi(this);

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

    // add border
    borderRectangle = new borderRect(0.0, 0.0, hSize, vSize, NULL);
    borderRectangle->setPen(QPen(QBrush(QColor(80, 80, 80)), 4.0, Qt::SolidLine, Qt::SquareCap));
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

    connect(ui->actionImport, SIGNAL(triggered()), this, SLOT(imageSelectorWindow()));
    connect(ui->rolesListWidget, SIGNAL(currentRowChanged(int)), SLOT(displayNewRectItem(int)));
    connect(ui->clearSceneItemsButton, SIGNAL(clicked()), this, SLOT(removeAllDisplayedItems()));
    connect(ui->switchPortraitLandscapeButton, SIGNAL(clicked()), this, SLOT(togglePortraitLandscapeMode()));
    connect(ui->switchSingleMultipleImagesButton, SIGNAL(clicked()), this, SLOT(toggleSingleMultipleImageMode()));
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
    ImageSelector im_select_window(rolesList, imageItems, importedImages, this);
    im_select_window.exec();

    updateRolesListWidgetColor();

    this->centralWidget()->setFocusPolicy(Qt::StrongFocus);
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

            // note that you can access xSize or ySize from inside the
            resizeRect * r_rect = new resizeRect(pos.x(), pos.y(), size.width(), size.height(), NULL);
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

void MainWindow::togglePortraitLandscapeMode()
{
    if(ratio_mode==LANDSCAPE)
    {
        ratio_mode = PORTRAIT;
        ui->switchPortraitLandscapeButton->setIcon(QIcon(":/quick_access/icons/landscape-icon.png"));
    }
    else if(ratio_mode==PORTRAIT)
    {
        ui->switchPortraitLandscapeButton->setIcon(QIcon(":/quick_access/icons/portrait-icon.png"));
        ratio_mode = LANDSCAPE;
    }

    qreal temp = hSize;
    hSize = vSize;
    vSize = temp;

    hideDisplayedItems();
    QThread::msleep(item_opaque_animation_delay);

    // resize the border rect
    QPropertyAnimation * border_animation = new QPropertyAnimation(borderRectangle, "size");
    border_animation->setDuration(border_resize_animation_delay);
    border_animation->setStartValue(borderRectangle->getSize());
    border_animation->setEndValue(QSizeF(hSize, vSize));

    // animate resizing of scene rect, so the rectangle will not visualy just jump to another position like
    QPropertyAnimation * scene_rect_animation = new QPropertyAnimation(view, "sceneRect");
    scene_rect_animation->setDuration(border_resize_animation_delay);
    scene_rect_animation->setStartValue(view->sceneRect());
    scene_rect_animation->setEndValue(QRectF(-offset, -offset, hSize+offset*2.0, vSize+offset*2.0));

    scene_rect_animation->start(QAbstractAnimation::DeleteWhenStopped);
    border_animation->start(QAbstractAnimation::DeleteWhenStopped);

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

    scene->update();
}

void MainWindow::toggleSingleMultipleImageMode()
{
    if(image_mode==SINGLE)
    {
        image_mode = MULTIPLE;
        ui->switchSingleMultipleImagesButton->setIcon(QIcon(":/quick_access/icons/single-image-icon.png"));
    }
    else if(image_mode==MULTIPLE)
    {
        ui->switchSingleMultipleImagesButton->setIcon(QIcon(":/quick_access/icons/multiple-images-icon.png"));
        image_mode = SINGLE;

        // if going to single mode, we need to delete all objects in scene currently visible
        removeAllDisplayedItems();
    }
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

void MainWindow::updateVisibleItems()
{
    // if data in image_handlers is changed, we need to call update function of each visible item, to immediately apply changes
    for(int i=0; i<displayedItems->count(); i++)
        displayedItems->at(i)->updateData();
}

void MainWindow::removeAllDisplayedItems()
{
    while(!displayedItems->isEmpty())
    {
        displayedItems->first()->deleteLater();
        displayedItems->removeFirst();
    }
}
