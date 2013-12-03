include(../common-project-config.pri)
include(../common-vars.pri)

QT       += dbus
QT       -= gui

TARGET = ubuntu-download-manager-client
TEMPLATE = lib

DEFINES += UBUNTUDOWNLOADMANAGERCLIENT_LIBRARY

SOURCES += ubuntu/download_manager/manager.cpp

public_headers = \
    ubuntu/download_manager/manager.h

private_headers = \
    ubuntu-download-manager-client_global.h

HEADERS +=\
    $$public_headers \
    $$private_headers

OTHER_FILES += \
    ubuntu-download-manager-client.pc.in

headers.files = $$public_headers

include(../common-installs-config.pri)

pkgconfig.files = ubuntu-download-manager-client.pc
include(../common-pkgconfig.pri)
INSTALLS += pkgconfig

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-common/ -lubuntu-download-manager-common

INCLUDEPATH += $$PWD/../ubuntu-download-manager-common
DEPENDPATH += $$PWD/../ubuntu-download-manager-common
