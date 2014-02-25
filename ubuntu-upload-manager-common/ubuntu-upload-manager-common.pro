#-------------------------------------------------
#
# Project created by QtCreator 2014-02-21T18:48:38
#
#-------------------------------------------------

QT       -= gui

TARGET = ubuntu-upload-manager-common
TEMPLATE = lib

DEFINES += UBUNTUUPLOADMANAGERCOMMON_LIBRARY

SOURCES += test.cpp

HEADERS += test.h\
        ubuntu-upload-manager-common_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
