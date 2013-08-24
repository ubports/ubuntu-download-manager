QT       += network dbus systeminfo
QT       -= gui

TARGET = ubuntudownloadmanager
QMAKE_CXXFLAGS += -std=c++0x -Werror
TEMPLATE = lib

DEFINES += APPDOWNLOADERLIB_LIBRARY
DEFINES += DEBUG

SOURCES += \
    application.cpp \
    dbus_connection.cpp \
    download.cpp \
    download_adaptor.cpp \
    download_daemon.cpp \
    download_manager.cpp \
    download_manager_adaptor.cpp \
    download_queue.cpp \
    logger.cpp \
    network_reply.cpp \
    process.cpp \
    process_factory.cpp \
    request_factory.cpp \
    single_download.cpp \
    system_network_info.cpp \
    timer.cpp \
    uuid_factory.cpp \
    xdg_basedir.cpp

HEADERS +=\
    app-downloader-lib_global.h \
    application.h \
    dbus_connection.h \
    download.h \
    download_adaptor.h \
    download_daemon.h \
    download_manager.h \
    download_manager_adaptor.h \
    download_queue.h \
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
    xdg_basedir.h

OTHER_FILES += \
    generate_adaptors.sh \
    com.canonical.applications.download.xml \
    com.canonical.applications.download_manager.xml

target.path = /usr/lib/
INSTALLS += target
