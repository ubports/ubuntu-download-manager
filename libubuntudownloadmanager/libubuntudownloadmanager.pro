QT       += network dbus systeminfo sql
QT       -= gui

TARGET = ubuntudownloadmanager
QMAKE_CXXFLAGS += -std=c++0x -Werror
TEMPLATE = lib

DEFINES += APPDOWNLOADERLIB_LIBRARY
# DEFINES += SHOW_TRACE use if you want more debug messages
DEFINES += DEBUG

SOURCES += \
    dbus_proxy.cpp \
    download.cpp \
    download_adaptor.cpp \
    download_daemon.cpp \
    downloads_db.cpp \
    download_factory.cpp \
    download_manager.cpp \
    download_manager_adaptor.cpp \
    download_queue.cpp \
    download_struct.cpp \
    group_download.cpp \
    group_download_struct.cpp \
    group_download_adaptor.cpp \
    hash_algorithm.cpp \
    single_download.cpp \
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
    system/network_reply.cpp

HEADERS +=\
    app-downloader-lib_global.h \
    dbus_proxy.h \
    download.h \
    download_adaptor.h \
    downloads_db.h \
    download_daemon.h \
    download_factory.h \
    download_manager.h \
    download_manager_adaptor.h \
    download_queue.h \
    download_struct.h \
    group_download.h \
    group_download_struct.h \
    group_download_adaptor.h \
    hash_algorithm.h \
    metatypes.h \
    single_download.h \
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
    system/network_reply.h

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

