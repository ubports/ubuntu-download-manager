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
    downloads/test_download.cpp \
    downloads/test_daemon.cpp \
    downloads/test_download_manager.cpp \
    downloads/test_download_queue.cpp \
    downloads/test_group_download.cpp \
    downloads/test_downloads_db.cpp \
    downloads/test_download_factory.cpp \
    fakes/fake.cpp \
    fakes/fake_dbus_connection.cpp \
    fakes/fake_download.cpp \
    fakes/fake_network_reply.cpp \
    fakes/fake_qnetwork_reply.cpp \
    fakes/fake_request_factory.cpp \
    fakes/fake_download_queue.cpp \
    fakes/fake_uuid_factory.cpp \
    fakes/fake_system_network_info.cpp \
    fakes/fake_process.cpp \
    fakes/fake_process_factory.cpp \
    fakes/fake_application.cpp \
    fakes/fake_timer.cpp \
    fakes/fake_download_manager.cpp \
    fakes/fake_download_factory.cpp \
    fakes/fake_file_manager.cpp \
    fakes/fake_apparmor.cpp \
    system/test_network_reply.cpp \
    main.cpp \
    base_testcase.cpp

HEADERS += \
    downloads/test_download.h \
    downloads/test_daemon.h \
    downloads/test_download_manager.h \
    downloads/test_download_queue.h \
    downloads/test_group_download.h \
    downloads/test_downloads_db.h \
    downloads/test_download_factory.h \
    fakes/fake.h \
    fakes/fake_dbus_connection.h \
    fakes/fake_download.h \
    fakes/fake_network_reply.h \
    fakes/fake_qnetwork_reply.h \
    fakes/fake_request_factory.h \
    fakes/fake_download_queue.h \
    fakes/fake_uuid_factory.h \
    fakes/fake_system_network_info.h \
    fakes/fake_process.h \
    fakes/fake_process_factory.h \
    fakes/fake_application.h \
    fakes/fake_timer.h \
    fakes/fake_download_manager.h \
    fakes/fake_download_factory.h \
    fakes/fake_file_manager.h \
    fakes/fake_apparmor.h \
    system/test_network_reply.h \
    test_runner.h \
    base_testcase.h

LIBS += -L$$OUT_PWD/../libubuntudownloadmanager/ -lubuntudownloadmanager

INCLUDEPATH += $$PWD/../libubuntudownloadmanager
DEPENDPATH += $$PWD/../libubuntudownloadmanager

check.depends = $${TARGET}
check.commands = LD_LIBRARY_PATH=$$PWD/../libubuntudownloadmanager ./$${TARGET}
QMAKE_EXTRA_TARGETS += check

