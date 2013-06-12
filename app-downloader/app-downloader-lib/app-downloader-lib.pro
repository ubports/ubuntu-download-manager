#-------------------------------------------------
#
# Project created by QtCreator 2013-06-12T12:09:34
#
#-------------------------------------------------

QT       += network dbus

QT       -= gui

TARGET = app-downloader-lib
TEMPLATE = lib

DEFINES += APPDOWNLOADERLIB_LIBRARY

SOURCES += downloader_daemon.cpp \
    downloader_adaptor.cpp \
    application_download_adaptor.cpp

HEADERS += downloader_daemon.h\
        app-downloader-lib_global.h \
    downloader_adaptor.h \
    application_download_adaptor.h

OTHER_FILES += \
    com.canonical.applications.application_download.xml \
    com.canonical.applications.downloader.xml
