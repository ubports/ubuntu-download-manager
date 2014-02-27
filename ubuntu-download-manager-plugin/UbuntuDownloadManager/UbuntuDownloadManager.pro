include(../../common-project-config.pri)
include(../../common-vars.pri)

TEMPLATE = subdirs
SUBDIRS = modules/UbuntuDownloadManager

check.target = check
check.commands = qmltestrunner -import modules -platform ubuntu
check.depends = modules/UbuntuDownloadManager
QMAKE_EXTRA_TARGETS += check

