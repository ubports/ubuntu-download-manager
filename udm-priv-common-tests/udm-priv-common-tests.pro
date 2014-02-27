include( ../common-project-config.pri )
include( ../common-vars.pri )

QT       += network core testlib dbus systeminfo sql

QT       -= gui

TARGET = udm-priv-common-tests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += \
    ubuntu/transfers/tests/transfer.h \
    ubuntu/transfers/tests/test_download_queue.h

SOURCES += \
    ubuntu/transfers/tests/transfer.cpp \
    ubuntu/transfers/tests/test_download_queue.cpp \
    main.cpp

LIBS += -L$$OUT_PWD/../udm-common/ -ludm-common

INCLUDEPATH += $$PWD/../udm-common
DEPENDPATH += $$PWD/../udm-common

LIBS += -L$$OUT_PWD/../udm-testing/ -ludm-testing

INCLUDEPATH += $$PWD/../udm-testing
DEPENDPATH += $$PWD/../udm-testing

LIBS += -L$$OUT_PWD/../udm-priv-common/ -ludm-priv-common

INCLUDEPATH += $$PWD/../udm-priv-common
DEPENDPATH += $$PWD/../udm-priv-common

check.depends = $${TARGET}
check.commands = LD_LIBRARY_PATH=$$OUT_PWD/../udm-common:$$OUT_PWD/../udm-priv-common:$$OUT_PWD/../udm-testing ./$${TARGET}
QMAKE_EXTRA_TARGETS += check
