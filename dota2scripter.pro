#-------------------------------------------------
#
# Project created by QtCreator 2015-10-21T14:55:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dota2scripter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    addonselectdialog.cpp

HEADERS  += mainwindow.h \
    global.h \
    addonselectdialog.h

FORMS    += mainwindow.ui \
    luaeditor.ui \
    addonselectdialog.ui

RESOURCES += \
    icons.qrc
