include( ../common-project-config.pri )
include( ../common-vars.pri )

QT       += network core testlib dbus systeminfo sql
QT       -= gui

TARGET = ubuntu-download-manager-client-tests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += \
    test_group_manager_watch.h \
    test_download_manager_watch.h \
    test_manager.h \
    local_tree_testcase.h \
    test_download_watch.h \
    test_download.h

SOURCES += main.cpp \
    test_group_manager_watch.cpp \
    test_download_manager_watch.cpp \
    test_manager.cpp \
    local_tree_testcase.cpp \
    test_download_watch.cpp \
    test_download.cpp

copydata.commands = $(COPY_DIR) $$PWD/data $$OUT_PWD
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-common/ -lubuntu-download-manager-common

INCLUDEPATH += $$PWD/../ubuntu-download-manager-common
DEPENDPATH += $$PWD/../ubuntu-download-manager-common

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-client/ -lubuntu-download-manager-client

INCLUDEPATH += $$PWD/../ubuntu-download-manager-client
DEPENDPATH += $$PWD/../ubuntu-download-manager-client

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-test-lib/ -lubuntu-download-manager-test-lib

INCLUDEPATH += $$PWD/../ubuntu-download-manager-test-lib
DEPENDPATH += $$PWD/../ubuntu-download-manager-test-lib

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-priv/ -lubuntu-download-manager-priv

INCLUDEPATH += $$PWD/../ubuntu-download-manager-priv
DEPENDPATH += $$PWD/../ubuntu-download-manager-priv


check.depends = $${TARGET}
check.commands = LD_LIBRARY_PATH=$$OUT_PWD/../ubuntu-download-manager-common:$$OUT_PWD/../ubuntu-download-manager-client:$$OUT_PWD/../ubuntu-download-manager-test-lib:$$OUT_PWD/../ubuntu-download-manager-priv ./$${TARGET}
QMAKE_EXTRA_TARGETS += check
