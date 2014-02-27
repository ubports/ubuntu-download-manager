include( ../common-project-config.pri )
include( ../common-vars.pri )

QT       += network core testlib dbus systeminfo sql

QT       -= gui

TARGET = udm-priv-common-tests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#HEADERS += \
#ubuntu/transfers/tests/test_download_queue.h

SOURCES += \
#    ubuntu/transfers/tests/test_download_queue.cpp \
    main.cpp

LIBS += -L$$OUT_PWD/../udm-common/ -ludm-common

INCLUDEPATH += $$PWD/../udm-common
DEPENDPATH += $$PWD/../udm-common
