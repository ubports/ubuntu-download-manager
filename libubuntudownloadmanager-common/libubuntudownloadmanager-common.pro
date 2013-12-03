include( ../common-project-config.pri )
include( ../common-vars.pri )

QT       += dbus
QT       -= gui

TARGET = ubuntu-download-manager-common
TEMPLATE = lib

DEFINES += LIBUBUNTUDOWNLOADMANAGERCOMMON_LIBRARY

SOURCES += \
    ubuntu/download_manager/download_struct.cpp \
    ubuntu/download_manager/group_download_struct.cpp \
    ubuntu/download_manager/system/hash_algorithm.cpp

public_headers = \
    ubuntu/download_manager/metatypes.h \
    ubuntu/download_manager/download_struct.h \
    ubuntu/download_manager/group_download_struct.h

private_headers = \
    libubuntudownloadmanager-common_global.h \
    ubuntu/download_manager/system/hash_algorithm.h

HEADERS +=\
    $$public_headers \
    $$private_headers

headers.files = $$public_headers

include(../common-installs-config.pri)

pkgconfig.files = ubuntudownloadmanager-common.pc
include(../common-pkgconfig.pri)
INSTALLS += pkgconfig

OTHER_FILES += \
    ubuntudownloadmanager-common.pc.in
