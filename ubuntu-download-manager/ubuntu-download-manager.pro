#-------------------------------------------------
#
# Project created by QtCreator 2013-06-12T13:03:54
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ubuntu-download-manager
QMAKE_CXXFLAGS += -std=c++0x -Werror
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

dbus_activation.path = /usr/share/dbus-1/services/
dbus_activation.files = ubuntu-download-manager.service
INSTALLS += dbus_activation

target.path = /usr/bin/
INSTALLS += target

LIBS += -L$$OUT_PWD/../libubuntudownloadmanager/ -lubuntudownloadmanager

INCLUDEPATH += $$PWD/../libubuntudownloadmanager
DEPENDPATH += $$PWD/../libubuntudownloadmanager

OTHER_FILES += \
    ubuntu-download-manager.service
