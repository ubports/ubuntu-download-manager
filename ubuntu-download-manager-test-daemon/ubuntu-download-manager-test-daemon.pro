include( ../common-project-config.pri )
include( ../common-vars.pri )

QT       += core systeminfo

QT       -= gui

TARGET = ubuntu-download-manager-test-daemon
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

unix:!macx: LIBS += -L$$OUT_PWD/../ubuntu-download-manager-common/ -lubuntu-download-manager-common

INCLUDEPATH += $$PWD/../ubuntu-download-manager-common
DEPENDPATH += $$PWD/../ubuntu-download-manager-common

unix:!macx: LIBS += -L$$OUT_PWD/../ubuntu-download-manager-priv/ -lubuntu-download-manager-priv

INCLUDEPATH += $$PWD/../ubuntu-download-manager-priv
DEPENDPATH += $$PWD/../ubuntu-download-manager-priv

unix:!macx: LIBS += -L$$OUT_PWD/../ubuntu-download-manager-test-lib/ -lubuntu-download-manager-test-lib

INCLUDEPATH += $$PWD/../ubuntu-download-manager-test-lib
DEPENDPATH += $$PWD/../ubuntu-download-manager-test-lib
