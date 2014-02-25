include(common.pri)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    ubuntu-download-manager-common \
    udm-priv-common \
    ubuntu-download-manager-priv \
    ubuntu-download-manager \
    ubuntu-download-manager-client \
    ubuntu-download-manager-test-lib \
    ubuntu-download-manager-tests \
    ubuntu-download-manager-test-daemon \
    ubuntu-download-manager-client-tests \
    ubuntu-upload-manager \
    ubuntu-upload-manager-common \
    ubuntu-upload-manager-priv \
    udm-common

udm-priv-common.depends = ubuntu-download-manager-common

ubuntu-download-manager-priv.depends = udm-priv-common

ubuntu-download-manager.depends = ubuntu-download-manager-priv
ubuntu-upload-manager.depends = ubuntu-download-manager-priv

ubuntu-download-manager-client.depends = ubuntu-download-manager-common

ubuntu-download-manager-tests.depends += ubuntu-download-manager-test-lib
ubuntu-download-manager-tests.depends += ubuntu-download-manager-priv

ubuntu-download-manager-client-tests.depends += ubuntu-download-manager-test-daemon
ubuntu-download-manager-client-tests.depends += ubuntu-download-manager-test-lib
ubuntu-download-manager-client-tests.depends += ubuntu-download-manager-client
