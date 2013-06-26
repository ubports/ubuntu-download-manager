#-------------------------------------------------
#
# Project created by QtCreator 2013-06-12T13:03:54
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = app-downloader
QMAKE_CXXFLAGS += -std=c++0x
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../app-downloader-lib/release/ -lapp-downloader-lib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../app-downloader-lib/debug/ -lapp-downloader-lib
else:unix: LIBS += -L$$OUT_PWD/../app-downloader-lib/ -lapp-downloader-lib

INCLUDEPATH += $$PWD/../app-downloader-lib
DEPENDPATH += $$PWD/../app-downloader-lib
