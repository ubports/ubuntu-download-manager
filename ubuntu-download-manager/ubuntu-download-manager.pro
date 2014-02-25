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

LIBS += -L$$OUT_PWD/../udm-common/ -ludm-common

INCLUDEPATH += $$PWD/../udm-common
DEPENDPATH += $$PWD/../udm-common

LIBS += -L$$OUT_PWD/../udm-priv-common/ -ludm-priv-common

INCLUDEPATH += $$PWD/../udm-priv-common
DEPENDPATH += $$PWD/../udm-priv-common

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-priv/ -lubuntu-download-manager-priv

INCLUDEPATH += $$PWD/../ubuntu-download-manager-priv
DEPENDPATH += $$PWD/../ubuntu-download-manager-priv

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-common/ -lubuntu-download-manager-common

INCLUDEPATH += $$PWD/../ubuntu-download-manager-common
DEPENDPATH += $$PWD/../ubuntu-download-manager-common
