QT       += network dbus systeminfo
QT       -= gui

TARGET = libubuntudownloadmanager
QMAKE_CXXFLAGS += -std=c++0x -Werror
TEMPLATE = lib

DEFINES += APPDOWNLOADERLIB_LIBRARY

SOURCES += \
    download.cpp \
    download_daemon.cpp \
    download_manager.cpp \
    download_queue.cpp \
    xdg_basedir.cpp \
    request_factory.cpp \
    network_reply.cpp \
    system_network_info.cpp \
    dbus_connection.cpp \
    download_adaptor.cpp \
    download_manager_adaptor.cpp \
    uuid_factory.cpp

HEADERS +=\
    app-downloader-lib_global.h \
    download.h \
    download_daemon.h \
    download_manager.h \
    download_queue.h \
    xdg_basedir.h \
    request_factory.h \
    system_network_info.h \
    network_reply.h \
    dbus_connection.h \
    download_adaptor.h \
    download_manager_adaptor.h \
    uuid_factory.h

OTHER_FILES += \
    generate_adaptors.sh \
    com.canonical.applications.download.xml \
    com.canonical.applications.download_manager.xml
