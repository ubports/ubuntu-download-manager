include(../common-project-config.pri)
include(../common-vars.pri)

QT       += network dbus systeminfo sql
QT       -= gui

TARGET = ubuntu-download-manager-priv
TEMPLATE = lib

DEFINES += APPDOWNLOADERLIB_LIBRARY
# DEFINES += SHOW_TRACE use if you want more debug messages
DEFINES += DEBUG

SOURCES += \
    downloads/daemon.cpp \
    downloads/download.cpp \
    downloads/download_adaptor.cpp \
    downloads/downloads_db.cpp \
    downloads/factory.cpp \
    downloads/manager.cpp \
    downloads/download_manager_adaptor.cpp \
    downloads/queue.cpp \
    downloads/group_download.cpp \
    downloads/group_download_adaptor.cpp \
    downloads/file_download.cpp \
    downloads/sm_file_download.cpp \
    downloads/state_machines/download_sm.cpp \
    system/uuid_utils.cpp \
    system/uuid_factory.cpp \
    system/timer.cpp \
    system/system_network_info.cpp \
    system/request_factory.cpp \
    system/process_factory.cpp \
    system/process.cpp \
    system/logger.cpp \
    system/file_manager.cpp \
    system/application.cpp \
    system/apparmor.cpp \
    system/network_reply.cpp \
    system/dbus_proxy.cpp \
    downloads/state_machines/state.cpp \
    downloads/state_machines/final_state.cpp

HEADERS +=\
    app-downloader-lib_global.h \
    downloads/daemon.h \
    downloads/download.h \
    downloads/download_adaptor.h \
    downloads/downloads_db.h \
    downloads/factory.h \
    downloads/manager.h \
    downloads/download_manager_adaptor.h \
    downloads/queue.h \
    downloads/group_download.h \
    downloads/group_download_adaptor.h \
    downloads/file_download.h \
    downloads/sm_file_download.h \
    downloads/state_machines/download_sm.h \
    system/uuid_utils.h \
    system/uuid_factory.h \
    system/timer.h \
    system/system_network_info.h \
    system/request_factory.h \
    system/process_factory.h \
    system/process.h \
    system/logger.h \
    system/file_manager.h \
    system/application.h \
    system/apparmor.h \
    system/network_reply.h \
    system/dbus_proxy.h \
    downloads/state_machines/state.h \
    downloads/state_machines/final_state.h

OTHER_FILES += \
    generate_adaptors.sh \
    com.canonical.applications.download.xml \
    com.canonical.applications.download_manager.xml \
    com.canonical.applications.group_download.xml \
    org.freedesktop.DBus.xml \
    downloads/com.canonical.applications.group_download.xml \
    downloads/com.canonical.applications.download_manager.xml \
    downloads/com.canonical.applications.download.xml \
    downloads/generate_adaptors.sh

include(../common-installs-config.pri)

LIBS += -lnih-dbus

PKGCONFIG += dbus-1


LIBS += -L$$OUT_PWD/../ubuntu-download-manager-common/ -lubuntu-download-manager-common

INCLUDEPATH += $$PWD/../ubuntu-download-manager-common
DEPENDPATH += $$PWD/../ubuntu-download-manager-common
