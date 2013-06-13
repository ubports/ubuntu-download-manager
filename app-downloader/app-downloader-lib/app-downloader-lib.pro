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

SOURCES += \
    downloader_adaptor.cpp \
    application_download_adaptor.cpp \
    downloader.cpp \
    app_download.cpp \
    download_daemon.cpp

HEADERS +=\
        app-downloader-lib_global.h \
    downloader_adaptor.h \
    application_download_adaptor.h \
    downloader.h \
    app_download.h \
    download_daemon.h

OTHER_FILES += \
    com.canonical.applications.application_download.xml \
    com.canonical.applications.downloader.xml
