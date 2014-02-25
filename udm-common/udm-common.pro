include(../common-project-config.pri)
include(../common-vars.pri)

QT       += dbus 
QT       -= gui
DEFINES += NDEBUG

TARGET = udm-common
TEMPLATE = lib

SOURCES += \
    ubuntu/system/dbus_connection.cpp \
    ubuntu/system/hash_algorithm.cpp

HEADERS += \
    ubuntu/system/dbus_connection.h \
    ubuntu/system/hash_algorithm.h

include(../common-installs-config.pri)
