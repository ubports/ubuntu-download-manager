include(../common-project-config.pri)
include(../common-vars.pri)

QT       += dbus
QT       -= gui

TARGET = ubuntu-download-manager-client
TEMPLATE = lib

DEFINES += UBUNTUDOWNLOADMANAGERCLIENT_LIBRARY

SOURCES += ubuntu/download_manager/manager.cpp

HEADERS += ubuntu/download_manager/manager.h\
        ubuntu-download-manager-client_global.h

OTHER_FILES += \
    ubuntu-download-manager-client.pc.in
