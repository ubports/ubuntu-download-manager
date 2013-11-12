include(common.pri)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    libubuntudownloadmanager-common \
    libubuntudownloadmanager \
    ubuntu-download-manager \
    ubuntu-download-manager-tests

libubuntudownloadmanager.depends = libubuntudownloadmanager-common

ubuntu-download-manager.depends = libubuntudownloadmanager

ubuntu-download-manager-tests.depends = libubuntudownloadmanager
