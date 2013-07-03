#-------------------------------------------------
#
# Project created by QtCreator 2013-06-12T12:09:34
#
#-------------------------------------------------

QT       += network dbus

QT       -= gui

TARGET = libubuntudownloadmanager
QMAKE_CXXFLAGS += -std=c++0x -Werror
TEMPLATE = lib

DEFINES += APPDOWNLOADERLIB_LIBRARY

SOURCES += \
    download.cpp \
    download_daemon.cpp \
    download_manager.cpp \
    download_queue.cpp \
    xdg_basedir.cpp \
    request_factory.cpp \
    network_reply.cpp \
    dbus_connection.cpp \
    download_adaptor.cpp \
    download_manager_adaptor.cpp

HEADERS +=\
    app-downloader-lib_global.h \
    download.h \
    download_daemon.h \
    download_manager.h \
    download_queue.h \
    xdg_basedir.h \
    request_factory.h \
    network_reply.h \
    dbus_connection.h \
    download_adaptor.h \
    download_manager_adaptor.h

OTHER_FILES += \
    com.canonical.applications.application_download.xml \
    com.canonical.applications.downloader.xml \
    generate_adaptors.sh
