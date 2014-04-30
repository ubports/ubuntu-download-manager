include(../common-project-config.pri)
include(../common-vars.pri)

QT       += network dbus systeminfo sql
QT       -= gui
DEFINES += NDEBUG

TARGET = ubuntu-download-manager-priv
TEMPLATE = lib

DEFINES += APPDOWNLOADERLIB_LIBRARY

SOURCES += \
    ubuntu/udm/daemon.cpp \
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
    ubuntu/system/apn_proxy.cpp \
    ubuntu/system/filename_mutex.cpp

HEADERS +=\
    ubuntu/udm/daemon.h \
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
    ubuntu/system/apn_proxy.h \
    ubuntu/system/filename_mutex.h

include(../common-installs-config.pri)

LIBS += -lnih-dbus

PKGCONFIG += dbus-1
PKGCONFIG += libglog


LIBS += -L$$OUT_PWD/../ubuntu-download-manager-common/ -lubuntu-download-manager-common

INCLUDEPATH += $$PWD/../ubuntu-download-manager-common
DEPENDPATH += $$PWD/../ubuntu-download-manager-common
