include(../common-project-config.pri)
include(../common-vars.pri)

QT       += network dbus systeminfo sql
QT       -= gui
DEFINES += NDEBUG

TARGET = udm-priv-common
TEMPLATE = lib

DEFINES += UDMPRIVCOMMON_LIBRARY

SOURCES += \
    ubuntu/general/base_daemon.cpp \
    ubuntu/general/base_manager.cpp \
    ubuntu/system/uuid_utils.cpp \
    ubuntu/system/uuid_factory.cpp \
    ubuntu/system/timer.cpp \
    ubuntu/system/system_network_info.cpp \
    ubuntu/system/request_factory.cpp \
    ubuntu/system/process_factory.cpp \
    ubuntu/system/process.cpp \
    ubuntu/system/logger.cpp \
    ubuntu/system/file_manager.cpp \
    ubuntu/system/application.cpp \
    ubuntu/system/apparmor.cpp \
    ubuntu/system/network_reply.cpp \
    ubuntu/system/dbus_proxy.cpp \
    ubuntu/system/apn_request_factory.cpp \
    ubuntu/system/apn_proxy.cpp

HEADERS += \
    ubuntu/general/base_daemon.h \
    ubuntu/general/base_manager.h \
    ubuntu/general/adaptor_factory.h \
    ubuntu/general/manager_factory.h \
    ubuntu/system/uuid_utils.h \
    ubuntu/system/uuid_factory.h \
    ubuntu/system/timer.h \
    ubuntu/system/system_network_info.h \
    ubuntu/system/request_factory.h \
    ubuntu/system/process_factory.h \
    ubuntu/system/process.h \
    ubuntu/system/logger.h \
    ubuntu/system/file_manager.h \
    ubuntu/system/application.h \
    ubuntu/system/apparmor.h \
    ubuntu/system/network_reply.h \
    ubuntu/system/dbus_proxy.h \
    ubuntu/system/apn_request_factory.h \
    ubuntu/system/apn_proxy.h

include(../common-installs-config.pri)

LIBS += -lnih-dbus

PKGCONFIG += dbus-1
PKGCONFIG += libglog

LIBS += -L$$OUT_PWD/../udm-common/ -ludm-common

INCLUDEPATH += $$PWD/../udm-common
DEPENDPATH += $$PWD/../udm-common
