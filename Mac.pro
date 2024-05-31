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
    xmlutils.cpp

HEADERS  += mainwindow.h \
    toolutils.h \
    dialog.h \
    xmlutils.h \
    constlist.h

FORMS    += mainwindow.ui \
    dialog.ui

DISTFILES += \
    SystemTrayIconForm.ui.qml

RESOURCES += \
    resource.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lQt5Qmqtt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lQt5Qmqttd

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libQt5Qmqtt.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libQt5Qmqttd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/qmqtt.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/qmqttd.lib
