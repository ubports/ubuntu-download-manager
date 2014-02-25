#-------------------------------------------------
#
# Project created by QtCreator 2014-02-25T13:30:31
#
#-------------------------------------------------

QT       -= gui

TARGET = udm-common
TEMPLATE = lib

DEFINES += UDMCOMMON_LIBRARY

SOURCES += udmcommon.cpp

HEADERS += udmcommon.h\
        udm-common_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
