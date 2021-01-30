#-------------------------------------------------
#
# Project created by QtCreator 2020-10-10T11:09:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mac
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    toolutils.cpp

HEADERS  += mainwindow.h \
    toolutils.h

FORMS    += mainwindow.ui

DISTFILES += \
    SystemTrayIconForm.ui.qml
