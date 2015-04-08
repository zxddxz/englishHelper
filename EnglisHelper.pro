#-------------------------------------------------
#
# Project created by QtCreator 2015-03-31T17:33:54
#
#-------------------------------------------------

QT       += core gui
QT       += phonon

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EnglisHelper
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mwidget.cpp \
    mybutton.cpp \
    mlist.cpp \
    mlrc.cpp \
    mrepeat.cpp

HEADERS  += mainwindow.h \
    mwidget.h \
    mybutton.h \
    mlist.h \
    mlrc.h \
    mrepeat.h

FORMS    += mainwindow.ui \
    mwidget.ui

RESOURCES += \
    image.qrc
