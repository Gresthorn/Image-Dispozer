#include <QApplication>
#include "versions.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName(QString("Peter Mikula"));
    a.setApplicationName(QString("Image - Dispozer"));
    a.setApplicationVersion(QString("%1.%2.%3").arg(MAJOR_VERSION).arg(MINOR_VERSION).arg(BUILD_VERSION));

    MainWindow w;
    w.show();

    return a.exec();
}
