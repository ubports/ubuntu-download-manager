include(common.pri)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    ubuntu-download-manager-common \
    ubuntu-download-manager-priv \
    ubuntu-download-manager \
    ubuntu-download-manager-client \
    ubuntu-download-manager-test-lib \
    ubuntu-download-manager-tests \
    ubuntu-download-manager-common-tests \
    ubuntu-download-manager-test-daemon \
    ubuntu-download-manager-client-tests \
    ubuntu-download-manager-client-tests \
    ubuntu-download-manager-plugin

ubuntu-download-manager-priv.depends = ubuntu-download-manager-common

ubuntu-download-manager.depends = ubuntu-download-manager-priv

ubuntu-download-manager-client.depends = ubuntu-download-manager-common

ubuntu-download-manager-common-tests.depends += ubuntu-download-manager-test-daemon
ubuntu-download-manager-common-tests.depends += ubuntu-download-manager-common
ubuntu-download-manager-common-tests.depends += ubuntu-download-manager-test-lib

ubuntu-download-manager-tests.depends += ubuntu-download-manager-test-lib
ubuntu-download-manager-tests.depends += ubuntu-download-manager-priv

ubuntu-download-manager-client-tests.depends += ubuntu-download-manager-test-daemon
ubuntu-download-manager-client-tests.depends += ubuntu-download-manager-test-lib
ubuntu-download-manager-client-tests.depends += ubuntu-download-manager-client
