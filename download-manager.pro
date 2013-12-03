include(common.pri)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    ubuntu-download-manager-common \
    libubuntudownloadmanager \
    ubuntu-download-manager \
    ubuntu-download-manager-tests

libubuntudownloadmanager.depends = ubuntu-download-manager-common

ubuntu-download-manager.depends = libubuntudownloadmanager

ubuntu-download-manager-tests.depends = libubuntudownloadmanager
