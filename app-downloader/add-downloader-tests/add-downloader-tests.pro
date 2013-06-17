#-------------------------------------------------
#
# Project created by QtCreator 2013-06-17T13:18:37
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = add-downloader-tests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    test_app_download.cpp \
    test_download_daemon.cpp \
    test_downloader.cpp

HEADERS += \
    test_runner.h
