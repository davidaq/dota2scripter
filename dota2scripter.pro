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
    addonselectdialog.cpp \
    scripteditor.cpp \
    scripteditorlinenumbers.cpp \
    scriptdocument.cpp \
    scriptassistantlua.cpp \
    scriptassistant.cpp \
    tokenfinder.cpp \
    documentmanager.cpp

HEADERS  += mainwindow.h \
    global.h \
    addonselectdialog.h \
    scripteditor.h \
    scripteditorlinenumbers.h \
    scriptdocument.h \
    scriptassistantlua.h \
    scriptassistant.h \
    tokenfinder.h \
    documentmanager.h

FORMS    += mainwindow.ui \
    luaeditor.ui \
    addonselectdialog.ui

RESOURCES += \
    icons.qrc
