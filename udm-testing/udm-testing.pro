include( ../common-project-config.pri )
include( ../common-vars.pri )

QT       += network core testlib dbus systeminfo sql
QT       -= gui

TARGET = udm-testing
TEMPLATE = lib

DEFINES += UDMTESTING_LIBRARY

SOURCES += \
     ubuntu/transfers/tests/base_testcase.cpp \
     ubuntu/transfers/tests/fake.cpp \
     ubuntu/transfers/tests/system/apn_request_factory.cpp \
     ubuntu/transfers/tests/system/apparmor.cpp \
     ubuntu/transfers/tests/system/application.cpp \
     ubuntu/transfers/tests/system/dbus_connection.cpp \
     ubuntu/transfers/tests/system/file_manager.cpp \
     ubuntu/transfers/tests/system/filename_mutex.cpp \
     ubuntu/transfers/tests/system/network_reply.cpp \
     ubuntu/transfers/tests/system/process.cpp \
     ubuntu/transfers/tests/system/process_factory.cpp \
     ubuntu/transfers/tests/system/qnetwork_reply.cpp \
     ubuntu/transfers/tests/system/request_factory.cpp \
     ubuntu/transfers/tests/system/system_network_info.cpp \
     ubuntu/transfers/tests/system/timer.cpp \
     ubuntu/transfers/tests/system/uuid_factory.cpp

HEADERS += \
     ubuntu/transfers/tests/base_testcase.h \
     ubuntu/transfers/tests/fake.h \
     ubuntu/transfers/tests/test_runner.h \
     ubuntu/transfers/tests/system/apn_request_factory.h \
     ubuntu/transfers/tests/system/apparmor.h \
     ubuntu/transfers/tests/system/application.h \
     ubuntu/transfers/tests/system/dbus_connection.h \
     ubuntu/transfers/tests/system/file_manager.h \
     ubuntu/transfers/tests/system/filename_mutex.h \
     ubuntu/transfers/tests/system/network_reply.h \
     ubuntu/transfers/tests/system/process.h \
     ubuntu/transfers/tests/system/process_factory.h \
     ubuntu/transfers/tests/system/qnetwork_reply.h \
     ubuntu/transfers/tests/system/request_factory.h \
     ubuntu/transfers/tests/system/system_network_info.h \
     ubuntu/transfers/tests/system/timer.h \
     ubuntu/transfers/tests/system/uuid_factory.h

LIBS += -L$$OUT_PWD/../udm-common/ -ludm-common

INCLUDEPATH += $$PWD/../udm-common
DEPENDPATH += $$PWD/../udm-common

LIBS += -L$$OUT_PWD/../udm-priv-common/ -ludm-priv-common

INCLUDEPATH += $$PWD/../udm-priv-common
DEPENDPATH += $$PWD/../udm-priv-common

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-common/ -lubuntu-download-manager-common
