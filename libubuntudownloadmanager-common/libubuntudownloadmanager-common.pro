#-------------------------------------------------
#
# Project created by QtCreator 2013-11-08T19:10:55
#
#-------------------------------------------------

QT       += dbus
QT       -= gui

TARGET = ubuntudownloadmanager-common
TEMPLATE = lib

DEFINES += LIBUBUNTUDOWNLOADMANAGERCOMMON_LIBRARY

SOURCES += \
    ubuntu/download_manager/download_struct.cpp \
    ubuntu/download_manager/group_download_struct.cpp \
    ubuntu/download_manager/system/hash_algorithm.cpp

HEADERS +=\
    libubuntudownloadmanager-common_global.h \
    ubuntu/download_manager/metatypes.h \
    ubuntu/download_manager/download_struct.h \
    ubuntu/download_manager/group_download_struct.h \
    ubuntu/download_manager/system/hash_algorithm.h

target.path = /usr/lib/
INSTALLS += target
