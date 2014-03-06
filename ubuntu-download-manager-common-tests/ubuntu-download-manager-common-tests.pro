include( ../common-project-config.pri )
include( ../common-vars.pri )

QT       += core testlib network
QT       -= gui

TARGET = ubuntu-download-manager-common-tests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += \
    test_metadata.h

SOURCES += main.cpp \
    test_metadata.cpp

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-common/ -lubuntu-download-manager-common

INCLUDEPATH += $$PWD/../ubuntu-download-manager-common
DEPENDPATH += $$PWD/../ubuntu-download-manager-common

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-test-lib/ -lubuntu-download-manager-test-lib

INCLUDEPATH += $$PWD/../ubuntu-download-manager-test-lib
DEPENDPATH += $$PWD/../ubuntu-download-manager-test-lib

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-priv/ -lubuntu-download-manager-priv

INCLUDEPATH += $$PWD/../ubuntu-download-manager-priv
DEPENDPATH += $$PWD/../ubuntu-download-manager-priv

check.depends = $${TARGET}
check.commands = LD_LIBRARY_PATH=$$OUT_PWD/../ubuntu-download-manager-common:$$OUT_PWD/../ubuntu-download-manager-test-lib:$$OUT_PWD/../ubuntu-download-manager-priv ./$${TARGET}
QMAKE_EXTRA_TARGETS += check
