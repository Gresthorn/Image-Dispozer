#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setOrganizationName(QString("Peter Mikula"));
    QApplication::setApplicationName(QString("Image - Dispozer"));
    QApplication::setApplicationVersion(QString("1.0.0"));

    MainWindow w;
    w.show();

    return a.exec();
}
