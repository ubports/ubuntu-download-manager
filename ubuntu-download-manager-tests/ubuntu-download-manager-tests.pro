#-------------------------------------------------
#
# Project created by QtCreator 2013-06-17T13:18:37
#
#-------------------------------------------------

QT       += network core testlib dbus systeminfo sql

QT       -= gui

TARGET = ubuntu-download-manager-tests
QMAKE_CXXFLAGS += -std=c++0x -Werror
CONFIG   += console
CONFIG   -= app_bundle

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
    test_xdg_basedir.cpp \
    fake_download_queue.cpp \
    fake_uuid_factory.cpp \
    fake_system_network_info.cpp \
    fake_process.cpp \
    fake_process_factory.cpp \
    fake_application.cpp \
    fake_timer.cpp \
    fake_download_manager.cpp \
    test_group_download.cpp \
    fake_download_factory.cpp \
    fake_file_manager.cpp \
    test_downloads_db.cpp \
    test_download_factory.cpp \
    fake_apparmor.cpp

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
    test_xdg_basedir.h \
    fake_download_queue.h \
    fake_uuid_factory.h \
    fake_system_network_info.h \
    fake_process.h \
    fake_process_factory.h \
    fake_application.h \
    fake_timer.h \
    fake_download_manager.h \
    test_group_download.h \
    fake_download_factory.h \
    fake_file_manager.h \
    test_downloads_db.h \
    test_download_factory.h \
    fake_apparmor.h

LIBS += -L$$OUT_PWD/../libubuntudownloadmanager/ -lubuntudownloadmanager

INCLUDEPATH += $$PWD/../libubuntudownloadmanager
DEPENDPATH += $$PWD/../libubuntudownloadmanager

check.depends = $${TARGET}
check.commands = LD_LIBRARY_PATH=$$PWD/../libubuntudownloadmanager ./$${TARGET}
QMAKE_EXTRA_TARGETS += check

