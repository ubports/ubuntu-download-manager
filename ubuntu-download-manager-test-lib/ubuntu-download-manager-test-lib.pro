include( ../common-project-config.pri )
include( ../common-vars.pri )

QT       += network core testlib dbus systeminfo sql
QT       -= gui

TARGET = ubuntu-download-manager-test-lib
TEMPLATE = lib

DEFINES += UBUNTUDOWNLOADMANAGERTESTLIB_LIBRARY

SOURCES += ubuntu/download_manager/tests/base_testcase.cpp \
           ubuntu/download_manager/tests/fake.cpp \
           ubuntu/download_manager/tests/client/daemon_testcase.cpp \
           ubuntu/download_manager/tests/server/apparmor.cpp \
           ubuntu/download_manager/tests/server/application.cpp \
           ubuntu/download_manager/tests/server/dbus_connection.cpp \
           ubuntu/download_manager/tests/server/download.cpp \
           ubuntu/download_manager/tests/server/factory.cpp \
           ubuntu/download_manager/tests/server/file_manager.cpp \
           ubuntu/download_manager/tests/server/manager.cpp \
           ubuntu/download_manager/tests/server/network_reply.cpp \
           ubuntu/download_manager/tests/server/process.cpp \
           ubuntu/download_manager/tests/server/process_factory.cpp \
           ubuntu/download_manager/tests/server/qnetwork_reply.cpp \
           ubuntu/download_manager/tests/server/queue.cpp \
           ubuntu/download_manager/tests/server/request_factory.cpp \
           ubuntu/download_manager/tests/server/sm_file_download.cpp \
           ubuntu/download_manager/tests/server/system_network_info.cpp \
           ubuntu/download_manager/tests/server/timer.cpp \
           ubuntu/download_manager/tests/server/uuid_factory.cpp \
    ubuntu/download_manager/tests/dbus_service.cpp

HEADERS += ubuntu/download_manager/tests/base_testcase.h \
           ubuntu/download_manager/tests/fake.h\
           ubuntu/download_manager/tests/test_runner.h \
           ubuntu/download_manager/tests/client/daemon_testcase.h \
           ubuntu/download_manager/tests/server/apparmor.h \
           ubuntu/download_manager/tests/server/application.h \
           ubuntu/download_manager/tests/server/dbus_connection.h \
           ubuntu/download_manager/tests/server/download.h \
           ubuntu/download_manager/tests/server/factory.h \
           ubuntu/download_manager/tests/server/file_manager.h \
           ubuntu/download_manager/tests/server/manager.h \
           ubuntu/download_manager/tests/server/network_reply.h \
           ubuntu/download_manager/tests/server/process.h \
           ubuntu/download_manager/tests/server/process_factory.h \
           ubuntu/download_manager/tests/server/qnetwork_reply.h \
           ubuntu/download_manager/tests/server/queue.h \
           ubuntu/download_manager/tests/server/request_factory.h \
           ubuntu/download_manager/tests/server/sm_file_download.h \
           ubuntu/download_manager/tests/server/system_network_info.h \
           ubuntu/download_manager/tests/server/timer.h \
           ubuntu/download_manager/tests/server/uuid_factory.h \
    ubuntu/download_manager/tests/dbus_service.h

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-common/ -lubuntu-download-manager-common

INCLUDEPATH += $$PWD/../ubuntu-download-manager-common
DEPENDPATH += $$PWD/../ubuntu-download-manager-common

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-priv/ -lubuntu-download-manager-priv

INCLUDEPATH += $$PWD/../ubuntu-download-manager-priv
DEPENDPATH += $$PWD/../ubuntu-download-manager-priv
