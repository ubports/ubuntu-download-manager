include(../common-project-config.pri)
include(../common-vars.pri)

QT       += network dbus systeminfo sql
QT       -= gui
DEFINES += NDEBUG

TARGET = ubuntu-download-manager-priv
TEMPLATE = lib

DEFINES += APPDOWNLOADERLIB_LIBRARY

SOURCES += \
    ubuntu/downloads/daemon.cpp \
    ubuntu/downloads/download.cpp \
    ubuntu/downloads/download_adaptor.cpp \
    ubuntu/downloads/downloads_db.cpp \
    ubuntu/downloads/factory.cpp \
    ubuntu/downloads/manager.cpp \
    ubuntu/downloads/download_manager_adaptor.cpp \
    ubuntu/downloads/queue.cpp \
    ubuntu/downloads/group_download.cpp \
    ubuntu/downloads/group_download_adaptor.cpp \
    ubuntu/downloads/file_download.cpp \
    ubuntu/downloads/sm_file_download.cpp \
    ubuntu/downloads/state_machines/download_sm.cpp \
    ubuntu/downloads/state_machines/state.cpp \
    ubuntu/downloads/state_machines/final_state.cpp \
    ubuntu/downloads/mms_file_download.cpp \
    ubuntu/downloads/download_adaptor_factory.cpp \
    ubuntu/downloads/download_manager_factory.cpp \
    ubuntu/transfers/base_daemon.cpp \
    ubuntu/transfers/base_manager.cpp \
    ubuntu/transfers/system/uuid_utils.cpp \
    ubuntu/transfers/system/uuid_factory.cpp \
    ubuntu/transfers/system/timer.cpp \
    ubuntu/transfers/system/system_network_info.cpp \
    ubuntu/transfers/system/request_factory.cpp \
    ubuntu/transfers/system/process_factory.cpp \
    ubuntu/transfers/system/process.cpp \
    ubuntu/transfers/system/logger.cpp \
    ubuntu/transfers/system/file_manager.cpp \
    ubuntu/transfers/system/application.cpp \
    ubuntu/transfers/system/apparmor.cpp \
    ubuntu/transfers/system/network_reply.cpp \
    ubuntu/transfers/system/dbus_proxy.cpp \
    ubuntu/transfers/system/apn_request_factory.cpp \
    ubuntu/transfers/system/apn_proxy.cpp

HEADERS +=\
    ubuntu/downloads/daemon.h \
    ubuntu/downloads/download.h \
    ubuntu/downloads/download_adaptor.h \
    ubuntu/downloads/downloads_db.h \
    ubuntu/downloads/factory.h \
    ubuntu/downloads/manager.h \
    ubuntu/downloads/download_manager_adaptor.h \
    ubuntu/downloads/queue.h \
    ubuntu/downloads/group_download.h \
    ubuntu/downloads/group_download_adaptor.h \
    ubuntu/downloads/file_download.h \
    ubuntu/downloads/sm_file_download.h \
    ubuntu/downloads/state_machines/download_sm.h \
    ubuntu/downloads/state_machines/state.h \
    ubuntu/downloads/state_machines/final_state.h \
    ubuntu/downloads/mms_file_download.h \
    ubuntu/transfers/base_daemon.h \
    ubuntu/transfers/base_manager.h \
    ubuntu/transfers/adaptor_factory.h \
    ubuntu/transfers/manager_factory.h \
    ubuntu/downloads/download_adaptor_factory.h \
    ubuntu/downloads/download_manager_factory.h \
    ubuntu/transfers/system/uuid_utils.h \
    ubuntu/transfers/system/uuid_factory.h \
    ubuntu/transfers/system/timer.h \
    ubuntu/transfers/system/system_network_info.h \
    ubuntu/transfers/system/request_factory.h \
    ubuntu/transfers/system/process_factory.h \
    ubuntu/transfers/system/process.h \
    ubuntu/transfers/system/logger.h \
    ubuntu/transfers/system/file_manager.h \
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


LIBS += -L$$OUT_PWD/../ubuntu-download-manager-common/ -lubuntu-download-manager-common

INCLUDEPATH += $$PWD/../ubuntu-download-manager-common
DEPENDPATH += $$PWD/../ubuntu-download-manager-common
