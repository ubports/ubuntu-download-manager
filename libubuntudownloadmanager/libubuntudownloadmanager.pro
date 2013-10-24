QT       += network dbus systeminfo sql
QT       -= gui

TARGET = ubuntudownloadmanager
QMAKE_CXXFLAGS += -std=c++0x -Werror
TEMPLATE = lib

DEFINES += APPDOWNLOADERLIB_LIBRARY
# DEFINES += SHOW_TRACE use if you want more debug messages
DEFINES += DEBUG

SOURCES += \
    downloads/download.cpp \
    downloads/download_adaptor.cpp \
    downloads/download_daemon.cpp \
    downloads/downloads_db.cpp \
    downloads/download_factory.cpp \
    downloads/download_manager.cpp \
    downloads/download_manager_adaptor.cpp \
    downloads/download_queue.cpp \
    downloads/download_struct.cpp \
    downloads/group_download.cpp \
    downloads/group_download_struct.cpp \
    downloads/group_download_adaptor.cpp \
    downloads/single_download.cpp \
    system/hash_algorithm.cpp \
    system/uuid_utils.cpp \
    system/uuid_factory.cpp \
    system/timer.cpp \
    system/system_network_info.cpp \
    system/request_factory.cpp \
    system/process_factory.cpp \
    system/process.cpp \
    system/logger.cpp \
    system/file_manager.cpp \
    system/dbus_connection.cpp \
    system/application.cpp \
    system/apparmor.cpp \
    system/network_reply.cpp \
    system/dbus_proxy.cpp

HEADERS +=\
    app-downloader-lib_global.h \
    downloads/download.h \
    downloads/download_adaptor.h \
    downloads/downloads_db.h \
    downloads/download_daemon.h \
    downloads/download_factory.h \
    downloads/download_manager.h \
    downloads/download_manager_adaptor.h \
    downloads/download_queue.h \
    downloads/download_struct.h \
    downloads/group_download.h \
    downloads/group_download_struct.h \
    downloads/group_download_adaptor.h \
    downloads/single_download.h \
    system/hash_algorithm.h \
    system/uuid_utils.h \
    system/uuid_factory.h \
    system/timer.h \
    system/system_network_info.h \
    system/request_factory.h \
    system/process_factory.h \
    system/process.h \
    system/logger.h \
    system/file_manager.h \
    system/dbus_connection.h \
    system/application.h \
    system/apparmor.h \
    system/network_reply.h \
    system/dbus_proxy.h \
    metatypes.h

OTHER_FILES += \
    generate_adaptors.sh \
    com.canonical.applications.download.xml \
    com.canonical.applications.download_manager.xml \
    com.canonical.applications.group_download.xml \
    org.freedesktop.DBus.xml

target.path = /usr/lib/
INSTALLS += target

LIBS += -lnih-dbus

CONFIG += link_pkgconfig
PKGCONFIG += dbus-1

