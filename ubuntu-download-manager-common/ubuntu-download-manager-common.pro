include( ../common-project-config.pri )
include( ../common-vars.pri )

QT       += dbus network
QT       -= gui

TARGET = ubuntu-download-manager-common
TEMPLATE = lib

DEFINES += LIBUBUNTUDOWNLOADMANAGERCOMMON_LIBRARY

SOURCES += \
    ubuntu/download_manager/download_struct.cpp \
    ubuntu/download_manager/group_download_struct.cpp \
    ubuntu/download_manager/system/dbus_connection.cpp \
    ubuntu/download_manager/system/hash_algorithm.cpp \
    ubuntu/download_manager/http_error_struct.cpp \
    ubuntu/download_manager/network_error_struct.cpp \
    ubuntu/download_manager/process_error_struct.cpp \
    ubuntu/download_manager/metadata.cpp

public_headers = \
    ubuntu/download_manager/common.h \
    ubuntu/download_manager/metatypes.h \
    ubuntu/download_manager/download_struct.h \
    ubuntu/download_manager/group_download_struct.h \
    ubuntu/download_manager/http_error_struct.h \
    ubuntu/download_manager/network_error_struct.h \
    ubuntu/download_manager/process_error_struct.h \
    ubuntu/download_manager/metadata.h

private_headers = \
    ubuntu/download_manager/system/dbus_connection.h \
    ubuntu/download_manager/system/hash_algorithm.h

HEADERS +=\
    $$public_headers \
    $$private_headers

headers.files = $$public_headers

include(../common-installs-config.pri)

pkgconfig.files = ubuntu-download-manager-common.pc
include(../common-pkgconfig.pri)
INSTALLS += pkgconfig

OTHER_FILES += \
    ubuntu-download-manager-common.pc.in
