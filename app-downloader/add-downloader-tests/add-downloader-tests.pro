#-------------------------------------------------
#
# Project created by QtCreator 2013-06-17T13:18:37
#
#-------------------------------------------------

QT       += network core testlib

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
    test_runner.h \
    test_app_download.h \
    test_download_daemon.h \
    test_downloader.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../app-downloader-lib/release/ -lapp-downloader-lib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../app-downloader-lib/debug/ -lapp-downloader-lib
else:unix: LIBS += -L$$OUT_PWD/../app-downloader-lib/ -lapp-downloader-lib

INCLUDEPATH += $$PWD/../app-downloader-lib
DEPENDPATH += $$PWD/../app-downloader-lib
