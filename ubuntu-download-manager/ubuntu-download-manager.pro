#-------------------------------------------------
#
# Project created by QtCreator 2013-06-12T13:03:54
#
#-------------------------------------------------

QT       += core systeminfo

QT       -= gui

TARGET = ubuntu-download-manager
QMAKE_CXXFLAGS += -std=c++0x -Werror
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

dbus_session_activation.path = /usr/share/dbus-1/services/
dbus_session_activation.files = ubuntu-download-manager.service
INSTALLS += dbus_session_activation

dbus_system_activation.path = /usr/share/dbus-1/system-services/
dbus_system_activation.files = com.canonical.applications.Downloader.service
INSTALLS += dbus_system_activation

dbus_system_conf.path = /etc/dbus-1/system.d/
dbus_system_conf.files = com.canonical.applications.Downloader.conf
INSTALLS += dbus_system_conf

target.path = /usr/bin/
INSTALLS += target

LIBS += -L$$OUT_PWD/../libubuntudownloadmanager/ -lubuntudownloadmanager

INCLUDEPATH += $$PWD/../libubuntudownloadmanager
DEPENDPATH += $$PWD/../libubuntudownloadmanager

OTHER_FILES += \
    ubuntu-download-manager.service \
    com.canonical.applications.Downloader.conf
