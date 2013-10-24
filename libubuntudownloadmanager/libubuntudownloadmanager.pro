QT       += network dbus systeminfo sql
QT       -= gui

TARGET = ubuntudownloadmanager
QMAKE_CXXFLAGS += -std=c++0x -Werror
TEMPLATE = lib

DEFINES += APPDOWNLOADERLIB_LIBRARY
# DEFINES += SHOW_TRACE use if you want more debug messages
DEFINES += DEBUG

SOURCES += \
    application.cpp \
    apparmor.cpp \
    dbus_connection.cpp \
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
    file_manager.cpp \
    group_download.cpp \
    group_download_struct.cpp \
    group_download_adaptor.cpp \
    hash_algorithm.cpp \
    logger.cpp \
    network_reply.cpp \
    process.cpp \
    process_factory.cpp \
    request_factory.cpp \
    single_download.cpp \
    system_network_info.cpp \
    timer.cpp \
    uuid_factory.cpp \
    uuid_utils.cpp

HEADERS +=\
    app-downloader-lib_global.h \
    apparmor.h \
    application.h \
    dbus_connection.h \
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
    file_manager.h \
    group_download.h \
    group_download_struct.h \
    group_download_adaptor.h \
    hash_algorithm.h \
    logger.h \
    metatypes.h \
    network_reply.h \
    process.h \
    process_factory.h \
    request_factory.h \
    single_download.h \
    system_network_info.h \
    timer.h \
    uuid_factory.h \
    uuid_utils.h

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

