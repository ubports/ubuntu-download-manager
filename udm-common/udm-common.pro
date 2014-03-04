include(../common-project-config.pri)
include(../common-vars.pri)

QT       += dbus network
QT       -= gui
DEFINES += NDEBUG

TARGET = udm-common
TEMPLATE = lib

SOURCES += \
    ubuntu/transfers/errors/auth_error_struct.cpp \
    ubuntu/transfers/errors/http_error_struct.cpp \
    ubuntu/transfers/errors/network_error_struct.cpp \
    ubuntu/transfers/errors/process_error_struct.cpp \
    ubuntu/transfers/system/logger.cpp \
    ubuntu/transfers/system/dbus_connection.cpp \
    ubuntu/transfers/system/hash_algorithm.cpp \
    ubuntu/transfers/system/logger.cpp

HEADERS += \
    ubuntu/transfers/errors/auth_error_struct.h \
    ubuntu/transfers/errors/http_error_struct.h \
    ubuntu/transfers/errors/network_error_struct.h \
    ubuntu/transfers/errors/process_error_struct.h \
    ubuntu/transfers/system/logger.h \
    ubuntu/transfers/system/dbus_connection.h \
    ubuntu/transfers/system/hash_algorithm.h \
    ubuntu/transfers/system/logger.h

include(../common-installs-config.pri)
