#-------------------------------------------------
#
# Project created by QtCreator 2013-06-12T12:09:34
#
#-------------------------------------------------

QT       += network dbus

QT       -= gui

TARGET = app-downloader-lib
QMAKE_CXXFLAGS += -std=c++0x -Werror
TEMPLATE = lib

DEFINES += APPDOWNLOADERLIB_LIBRARY

SOURCES += \
    downloader_adaptor.cpp \
    application_download_adaptor.cpp \
    download.cpp \
    downloader.cpp \
    download_daemon.cpp \
    download_queue.cpp \
    xdg_basedir.cpp \
    request_factory.cpp \
    network_reply.cpp \
    dbus_connection.cpp

HEADERS +=\
    app-downloader-lib_global.h \
    downloader_adaptor.h \
    application_download_adaptor.h \
    download.h \
    downloader.h \
    download_daemon.h \
    download_queue.h \
    xdg_basedir.h \
    request_factory.h \
    network_reply.h \
    dbus_connection.h

OTHER_FILES += \
    com.canonical.applications.application_download.xml \
    com.canonical.applications.downloader.xml
