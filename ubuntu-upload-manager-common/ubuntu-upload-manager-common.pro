include( ../common-project-config.pri )
include( ../common-vars.pri )

QT       += dbus network
QT       -= gui

TARGET = ubuntu-upload-manager-common
TEMPLATE = lib

DEFINES += UBUNTUUPLOADMANAGERCOMMON_LIBRARY

SOURCES += \
    ubuntu/upload_manager/upload_struct.cpp

public_headers = \
    ubuntu/upload_manager/upload_struct.h \
    ubuntu/upload_manager/metatypes.h

HEADERS +=\
    $$public_headers

headers.files = $$public_headers

include(../common-installs-config.pri)

pkgconfig.files = ubuntu-upload-manager-common.pc
include(../common-pkgconfig.pri)
INSTALLS += pkgconfig

OTHER_FILES += \
    ubuntu-upload-manager-common.pc.in

LIBS += -L$$OUT_PWD/../udm-common/ -ludm-common

INCLUDEPATH += $$PWD/../udm-common
DEPENDPATH += $$PWD/../udm-common
