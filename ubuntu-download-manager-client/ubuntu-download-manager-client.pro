include(../common-project-config.pri)
include(../common-vars.pri)

QT       += dbus
QT       -= gui

TARGET = ubuntu-download-manager-client
TEMPLATE = lib

DEFINES += UBUNTUDOWNLOADMANAGERCLIENT_LIBRARY

SOURCES += ubuntu/download_manager/manager.cpp \
           ubuntu/download_manager/download_interface.cpp \
           ubuntu/download_manager/manager_interface.cpp \
           ubuntu/download_manager/download.cpp \
           ubuntu/download_manager/group_download.cpp \
           ubuntu/download_manager/manager_pendingcall_watcher.cpp \
           ubuntu/download_manager/error.cpp \
           ubuntu/download_manager/pending_call_watcher.cpp \
           ubuntu/download_manager/download_pendingcall_watcher.cpp

public_headers = \
    ubuntu/download_manager/manager.h \
    ubuntu/download_manager/download.h \
    ubuntu/download_manager/group_download.h \
    ubuntu/download_manager/error.h

private_headers = \
    ubuntu/download_manager/download_interface.h \
    ubuntu/download_manager/manager_interface.h \
    ubuntu/download_manager/manager_pendingcall_watcher.h \
    ubuntu/download_manager/pending_call_watcher.h \
    ubuntu/download_manager/download_pendingcall_watcher.h

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
