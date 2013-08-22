QT       += network dbus systeminfo
QT       -= gui

TARGET = ubuntudownloadmanager
QMAKE_CXXFLAGS += -std=c++0x -Werror
TEMPLATE = lib

DEFINES += APPDOWNLOADERLIB_LIBRARY
DEFINES += DEBUG

SOURCES += \
    dbus_connection.cpp \
    download.cpp \
    download_adaptor.cpp \
    download_daemon.cpp \
    download_manager.cpp \
    download_manager_adaptor.cpp \
    download_queue.cpp \
    network_reply.cpp \
    request_factory.cpp \
    system_network_info.cpp \
    uuid_factory.cpp \
    xdg_basedir.cpp \
    process.cpp \
    process_factory.cpp \
    logger.cpp \
    application.cpp \
    timer.cpp

HEADERS +=\
    app-downloader-lib_global.h \
    dbus_connection.h \
    download.h \
    download_adaptor.h \
    download_daemon.h \
    download_manager.h \
    download_manager_adaptor.h \
    download_queue.h \
    network_reply.h \
    request_factory.h \
    system_network_info.h \
    uuid_factory.h \
    xdg_basedir.h \
    process.h \
    process_factory.h \
    metatypes.h \
    logger.h \
    application.h \
    timer.h

OTHER_FILES += \
    generate_adaptors.sh \
    com.canonical.applications.download.xml \
    com.canonical.applications.download_manager.xml

target.path = /usr/lib/
INSTALLS += target
