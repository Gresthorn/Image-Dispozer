#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initializeTreeItems();

    // create new scene and view
    scene = new imageScene(this);
    scene->setBackgroundBrush(QBrush(QColor(36, 38, 41), Qt::SolidPattern));
    view = new imageView(scene, this);
    // add border to the scene
    hSize = 800.0;
    vSize = 600.0;

    view->scale(1.0, -1.0);
    qreal offset = 6.0; // because of borders (when windows gets too small and scrollbars are to be appeared)
    view->setSceneRect(-offset, -offset, hSize+offset*2.0, vSize+offset*2.0); // +-offset is because of scrollbars which appear if windows is too small
    view->centerOn(hSize/2.0, vSize/2.0);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setInteractive(true);
    view->setMouseTracking(true);

    // add border
    borderRect = scene->addRect(0.0, 0.0, hSize, vSize, QPen(QBrush(QColor(80, 80, 80)), 4.0, Qt::SolidLine, Qt::SquareCap), QBrush(QColor(220, 220, 220)));

    // display view in mainWindow
    QVBoxLayout * sceneWidgetLayout = new QVBoxLayout;
    sceneWidgetLayout->addWidget(view);
    ui->sceneWidget->setLayout(sceneWidgetLayout);

    // set window size to try show up all rectangle
    if(hSize*0.6<1336 && vSize*0.6<768)
        this->setGeometry(0, 100, hSize+150.0, vSize+150.0);

    // TEST PURPOSES ONLY
    /*resizeRect * r_rect = new resizeRect(200, 200, 300, 300, NULL);
    image_handler * i_handler =  new image_handler(QString("XXX\\Lena.jpg"), 0, 0);
    r_rect->setPixmap(i_handler);
    scene->addItem(r_rect);*/

    connect(ui->clickMe, SIGNAL(clicked()), this, SLOT(imageSelectorWindow()));
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
    }
}

void MainWindow::imageSelectorWindow()
{
    // create dialog window for selecting and linking images to their appropriate roles
    ImageSelector im_select_window(rolesList, imageItems, importedImages, this);
    im_select_window.exec();
}
