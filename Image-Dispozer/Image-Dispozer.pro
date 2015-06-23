#-------------------------------------------------
#
# Project created by QtCreator 2015-06-09T18:37:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Image-Dispozer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imageselector.cpp \
    image_handler.cpp \
    rolestring.cpp \
    imagescene.cpp \
    imageview.cpp \
    resizerect.cpp \
    borderrect.cpp \
    infolabel.cpp \
    manualdatainput.cpp \
    contactdatadialog.cpp \
    maincfgdialog.cpp

HEADERS  += mainwindow.h \
    imageselector.h \
    image_handler.h \
    rolestring.h \
    imagescene.h \
    imageview.h \
    resizerect.h \
    borderrect.h \
    infolabel.h \
    manualdatainput.h \
    versions.h \
    contactdatadialog.h \
    maincfgdialog.h

FORMS    += mainwindow.ui \
    imageselector.ui \
    contactdatadialog.ui \
    maincfgdialog.ui

RESOURCES += \
    icons.qrc
