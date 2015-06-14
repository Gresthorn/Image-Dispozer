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
    resizerect.cpp

HEADERS  += mainwindow.h \
    imageselector.h \
    image_handler.h \
    rolestring.h \
    imagescene.h \
    imageview.h \
    resizerect.h

FORMS    += mainwindow.ui \
    imageselector.ui
