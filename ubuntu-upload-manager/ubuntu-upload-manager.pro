include( ../common-project-config.pri )
include( ../common-vars.pri )

QT       += core systeminfo

QT       -= gui

TARGET = ubuntu-upload-manager
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

dbus_session_activation.path = /usr/share/dbus-1/services/
dbus_session_activation.files = ubuntu-upload-manager.service
INSTALLS += dbus_session_activation

dbus_system_activation.path = /usr/share/dbus-1/system-services/
dbus_system_activation.files = com.canonical.applications.Uploader.service
INSTALLS += dbus_system_activation

dbus_system_conf.path = /etc/dbus-1/system.d/
dbus_system_conf.files = com.canonical.applications.Uploader.conf
INSTALLS += dbus_system_conf

include( ../common-installs-config.pri )

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-priv/ -lubuntu-download-manager-priv

INCLUDEPATH += $$PWD/../ubuntu-download-manager-priv
DEPENDPATH += $$PWD/../ubuntu-download-manager-priv

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-common/ -lubuntu-download-manager-common

INCLUDEPATH += $$PWD/../ubuntu-download-manager-common
DEPENDPATH += $$PWD/../ubuntu-download-manager-common
