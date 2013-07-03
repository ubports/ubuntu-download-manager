include(common.pri)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    libubuntudownloadmanager \
    ubuntu-download-manager \
    ubuntu-download-manager-tests

ubuntu-download-manager.depends = libubuntudownloadmanager

ubuntu-download-manager-tests.depends = libubuntudownloadmanager
