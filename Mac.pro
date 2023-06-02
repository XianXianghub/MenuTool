#-------------------------------------------------
#
# Project created by QtCreator 2020-10-10T11:09:49
#
#-------------------------------------------------

QT       += core gui network
RC_FILE = logo.rc
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mac
TEMPLATE = app
include(qxtglobalshortcut5-master/qxt.pri)


SOURCES += main.cpp\
        mainwindow.cpp \
    toolutils.cpp \
    dialog.cpp

HEADERS  += mainwindow.h \
    toolutils.h \
    dialog.h

FORMS    += mainwindow.ui \
    dialog.ui

DISTFILES += \
    SystemTrayIconForm.ui.qml

RESOURCES += \
    resource.qrc
