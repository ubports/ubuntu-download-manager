include(common.pri)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    udm-common \
    udm-priv-common \
    ubuntu-download-manager-common \
    ubuntu-download-manager-priv \
    ubuntu-download-manager \
    ubuntu-download-manager-client \
    udm-testing \
    ubuntu-download-manager-test-lib \
    ubuntu-download-manager-tests \
    ubuntu-download-manager-test-daemon \
    ubuntu-download-manager-client-tests \
    ubuntu-upload-manager-common \
    ubuntu-upload-manager-priv \
    ubuntu-upload-manager \
    udm-priv-common-tests

udm-priv-common.depends = udm-common

ubuntu-download-manager-priv.depends += udm-common
ubuntu-download-manager-priv.depends += udm-priv-common

ubuntu-download-manager.depends += udm-common
ubuntu-download-manager.depends += udm-priv-common
ubuntu-download-manager.depends += ubuntu-download-manager-priv

ubuntu-upload-manager.depends += udm-common
ubuntu-upload-manager.depends += udm-priv-common
ubuntu-upload-manager.depends += ubuntu-upload-manager-priv

ubuntu-download-manager-client.depends = udm-common
ubuntu-download-manager-client.depends = ubuntu-download-manager-common

ubuntu-download-manager-tests.depends += ubuntu-download-manager-test-lib
ubuntu-download-manager-tests.depends += ubuntu-download-manager-priv

ubuntu-download-manager-client-tests.depends += ubuntu-download-manager-test-daemon
ubuntu-download-manager-client-tests.depends += ubuntu-download-manager-test-lib
ubuntu-download-manager-client-tests.depends += ubuntu-download-manager-client
