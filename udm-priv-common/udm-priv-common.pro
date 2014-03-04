include(../common-project-config.pri)
include(../common-vars.pri)

QT       += network dbus systeminfo sql
QT       -= gui
DEFINES += NDEBUG

TARGET = udm-priv-common
TEMPLATE = lib

DEFINES += UDMPRIVCOMMON_LIBRARY

SOURCES += \
    ubuntu/transfers/base_daemon.cpp \
    ubuntu/transfers/base_manager.cpp \
    ubuntu/transfers/system/uuid_utils.cpp \
    ubuntu/transfers/system/uuid_factory.cpp \
    ubuntu/transfers/system/timer.cpp \
    ubuntu/transfers/system/system_network_info.cpp \
    ubuntu/transfers/system/request_factory.cpp \
    ubuntu/transfers/system/process_factory.cpp \
    ubuntu/transfers/system/process.cpp \
    ubuntu/transfers/system/file_manager.cpp \
    ubuntu/transfers/system/filename_mutex.cpp \
    ubuntu/transfers/system/application.cpp \
    ubuntu/transfers/system/apparmor.cpp \
    ubuntu/transfers/system/network_reply.cpp \
    ubuntu/transfers/system/dbus_proxy.cpp \
    ubuntu/transfers/system/apn_request_factory.cpp \
    ubuntu/transfers/system/apn_proxy.cpp

HEADERS += \
    ubuntu/transfers/base_daemon.h \
    ubuntu/transfers/base_manager.h \
    ubuntu/transfers/adaptor_factory.h \
    ubuntu/transfers/manager_factory.h \
    ubuntu/transfers/system/uuid_utils.h \
    ubuntu/transfers/system/uuid_factory.h \
    ubuntu/transfers/system/timer.h \
    ubuntu/transfers/system/system_network_info.h \
    ubuntu/transfers/system/request_factory.h \
    ubuntu/transfers/system/process_factory.h \
    ubuntu/transfers/system/process.h \
    ubuntu/transfers/system/file_manager.h \
    ubuntu/transfers/system/filename_mutex.h \
    ubuntu/transfers/system/application.h \
    ubuntu/transfers/system/apparmor.h \
    ubuntu/transfers/system/network_reply.h \
    ubuntu/transfers/system/dbus_proxy.h \
    ubuntu/transfers/system/apn_request_factory.h \
    ubuntu/transfers/system/apn_proxy.h

include(../common-installs-config.pri)

LIBS += -lnih-dbus

PKGCONFIG += dbus-1
PKGCONFIG += libglog

LIBS += -L$$OUT_PWD/../udm-common/ -ludm-common

INCLUDEPATH += $$PWD/../udm-common
DEPENDPATH += $$PWD/../udm-common
