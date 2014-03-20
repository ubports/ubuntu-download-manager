include( ../../../../common-project-config.pri )
include( ../../../../common-vars.pri )
TEMPLATE = lib
TARGET = UbuntuDownloadManager
QT += qml quick
CONFIG += qt plugin

#comment in the following line to disable traces
DEFINES += QT_NO_DEBUG_OUTPUT

TARGET = $$qtLibraryTarget($$TARGET)
uri = UbuntuDownloadManager

INCLUDEPATH += .

# Input
HEADERS += \
    backend.h \
    ubuntu_download_manager.h \
    download_error.h \
    single_download.h
SOURCES += backend.cpp \
    ubuntu_download_manager.cpp \
    download_error.cpp \
    single_download.cpp

API_VERSION = 0.1

# Install path for the plugin
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /).$$API_VERSION

target.path = $$installPath
INSTALLS += target

# find files
QMLDIR_FILE = qmldir

# make visible to qt creator
OTHER_FILES += $$QMLDIR_FILE 

# create install targets for files
qmldir.path = $$installPath
qmldir.files = $$QMLDIR_FILE

INSTALLS += qmldir

LIBS += -L$$OUT_PWD/../../../../ubuntu-download-manager-common/ -lubuntu-download-manager-common

INCLUDEPATH += $$PWD/../../../../ubuntu-download-manager-common
DEPENDPATH += $$PWD/../../../../ubuntu-download-manager-common

LIBS += -L$$OUT_PWD/../../../../ubuntu-download-manager-client/ -lubuntu-download-manager-client

INCLUDEPATH += $$PWD/../../../../ubuntu-download-manager-client
DEPENDPATH += $$PWD/../../../../ubuntu-download-manager-client
