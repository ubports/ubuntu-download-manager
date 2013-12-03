include(common.pri)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    ubuntu-download-manager-common \
    ubuntu-download-manager-priv \
    ubuntu-download-manager \
    ubuntu-download-manager-tests \
    ubuntu-download-manager-client

ubuntu-download-manager-priv.depends = ubuntu-download-manager-common

ubuntu-download-manager.depends = ubuntu-download-manager-priv

ubuntu-download-manager-tests.depends = ubuntu-download-manager-priv
