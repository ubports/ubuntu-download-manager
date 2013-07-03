#-------------------------------------------------
#
# Project created by QtCreator 2013-06-17T13:18:37
#
#-------------------------------------------------

QT       += network core testlib

QT       -= gui

TARGET = add-downloader-tests
QMAKE_CXXFLAGS += -std=c++0x -Werror
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    fake.cpp \
    fake_dbus_connection.cpp \
    fake_download.cpp \
    fake_network_reply.cpp \
    fake_qnetwork_reply.cpp \
    fake_request_factory.cpp \
    main.cpp \
    test_download.cpp \
    test_download_daemon.cpp \
    test_download_manager.cpp \
    test_download_queue.cpp \
    test_network_reply.cpp \
    test_xdg_basedir.cpp

HEADERS += \
    fake.h \
    fake_dbus_connection.h \
    fake_download.h \
    fake_network_reply.h \
    fake_qnetwork_reply.h \
    fake_request_factory.h \
    test_download.h \
    test_download_daemon.h \
    test_download_manager.h \
    test_download_queue.h \
    test_network_reply.h \
    test_runner.h \
    test_xdg_basedir.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../app-downloader-lib/release/ -lapp-downloader-lib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../app-downloader-lib/debug/ -lapp-downloader-lib
else:unix: LIBS += -L$$OUT_PWD/../app-downloader-lib/ -lapp-downloader-lib

INCLUDEPATH += $$PWD/../app-downloader-lib
DEPENDPATH += $$PWD/../app-downloader-lib
