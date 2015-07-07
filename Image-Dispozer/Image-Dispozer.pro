#-------------------------------------------------
#
# Project created by QtCreator 2015-06-09T18:37:29
#
#-------------------------------------------------

QT       += core gui multimedia

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
    maincfgdialog.cpp \
    smscontentsdialog.cpp \
    sound_handler.cpp \
    soundselector.cpp \
    ghostrect.cpp

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
    contactdatadialog.h \
    maincfgdialog.h \
    smscontentsdialog.h \
    sound_handler.h \
    soundselector.h \
    ghostrect.h

FORMS    += mainwindow.ui \
    imageselector.ui \
    contactdatadialog.ui \
    maincfgdialog.ui \
    smscontentsdialog.ui \
    soundselector.ui

RESOURCES += \
    icons.qrc

RC_FILE += iconrc.rc
