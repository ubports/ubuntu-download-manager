include( ../common-project-config.pri )
include( ../common-vars.pri )

QT       += core systeminfo

QT       -= gui

TARGET = ubuntu-download-manager
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

include( ../common-installs-config.pri )

LIBS += -L$$OUT_PWD/../libubuntudownloadmanager/ -lubuntudownloadmanager

INCLUDEPATH += $$PWD/../libubuntudownloadmanager
DEPENDPATH += $$PWD/../libubuntudownloadmanager

OTHER_FILES += \
    ubuntu-download-manager.service \
    com.canonical.applications.Downloader.conf

LIBS += -L$$OUT_PWD/../libubuntudownloadmanager-common/ -lubuntudownloadmanager-common

INCLUDEPATH += $$PWD/../libubuntudownloadmanager-common
DEPENDPATH += $$PWD/../libubuntudownloadmanager-common
