include(../common-project-config.pri)
include(../common-vars.pri)

QT       += dbus network
QT       -= gui
DEFINES += NDEBUG

TARGET = udm-common
TEMPLATE = lib

SOURCES += \
    ubuntu/errors/auth_error_struct.cpp \
    ubuntu/errors/http_error_struct.cpp \
    ubuntu/errors/network_error_struct.cpp \
    ubuntu/errors/process_error_struct.cpp \
    ubuntu/system/dbus_connection.cpp \
    ubuntu/system/hash_algorithm.cpp

HEADERS += \
    ubuntu/errors/auth_error_struct.h \
    ubuntu/errors/http_error_struct.h \
    ubuntu/errors/network_error_struct.h \
    ubuntu/errors/process_error_struct.h \
    ubuntu/system/dbus_connection.h \
    ubuntu/system/hash_algorithm.h

include(../common-installs-config.pri)
