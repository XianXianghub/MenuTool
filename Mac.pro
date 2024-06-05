#-------------------------------------------------
#
# Project created by QtCreator 2020-10-10T11:09:49
#
#-------------------------------------------------

QT       += core gui network xml
RC_FILE = logo.rc
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mac
TEMPLATE = app
include(qxtglobalshortcut5-master/qxt.pri)


SOURCES += main.cpp\
        mainwindow.cpp \
    toolutils.cpp \
    dialog.cpp \
    xmlutils.cpp \
    logger.cpp \
    sshtunnelconfigdialog.cpp \
    sshclient.cpp

HEADERS  += mainwindow.h \
    toolutils.h \
    dialog.h \
    xmlutils.h \
    constlist.h \
    logger.h \
    sshtunnelconfigdialog.h \
    sshclient.h
FORMS    += mainwindow.ui \
    dialog.ui \
    sshtunnelconfigdialog.ui

DISTFILES += \
    SystemTrayIconForm.ui.qml

RESOURCES += \
    resource.qrc


#LIBS += $$PWD/lib/libssh2.a

LIBS += -L$$PWD/lib/ -llibssh2
LIBS += -L$$PWD/lib/ -llibeay32
LIBS += -L$$PWD/lib/ -lssleay32
LIBS += -L$$PWD/lib/ -lzlib


#win32: LIBS += $$PWD/lib/libssh2.dll




INCLUDEPATH += $$PWD/include

