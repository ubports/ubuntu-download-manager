include( ../common-project-config.pri )
include( ../common-vars.pri )

QT       += network core testlib dbus systeminfo sql
QT       -= gui

TARGET = ubuntu-download-manager-test-lib
TEMPLATE = lib

DEFINES += UBUNTUDOWNLOADMANAGERTESTLIB_LIBRARY

SOURCES += \
    ubuntu/download_manager/tests/client/daemon_testcase.cpp \
    ubuntu/download_manager/tests/server/download.cpp \
    ubuntu/download_manager/tests/server/factory.cpp \
    ubuntu/download_manager/tests/server/manager.cpp \
    ubuntu/download_manager/tests/server/queue.cpp \
    ubuntu/download_manager/tests/server/sm_file_download.cpp \
    ubuntu/download_manager/tests/client/testing_daemon.cpp \
    ubuntu/download_manager/tests/client/testing_manager.cpp \
    ubuntu/download_manager/tests/client/testing_manager_adaptor.cpp \
    ubuntu/download_manager/tests/client/testing_interface.cpp \
    ubuntu/download_manager/tests/client/testing_file_download.cpp \
    ubuntu/download_manager/tests/server/group_download.cpp

HEADERS += \
    ubuntu/download_manager/tests/client/daemon_testcase.h \
    ubuntu/download_manager/tests/server/download.h \
    ubuntu/download_manager/tests/server/factory.h \
    ubuntu/download_manager/tests/server/manager.h \
    ubuntu/download_manager/tests/server/queue.h \
    ubuntu/download_manager/tests/server/sm_file_download.h \
    ubuntu/download_manager/tests/client/testing_daemon.h \
    ubuntu/download_manager/tests/client/testing_manager.h \
    ubuntu/download_manager/tests/client/testing_manager_adaptor.h \
    ubuntu/download_manager/tests/client/testing_interface.h \
    ubuntu/download_manager/tests/client/testing_file_download.h \
    ubuntu/download_manager/tests/server/group_download.h

LIBS += -L$$OUT_PWD/../udm-common/ -ludm-common

INCLUDEPATH += $$PWD/../udm-common
DEPENDPATH += $$PWD/../udm-common

LIBS += -L$$OUT_PWD/../udm-priv-common/ -ludm-priv-common

INCLUDEPATH += $$PWD/../udm-priv-common
DEPENDPATH += $$PWD/../udm-priv-common

LIBS += -L$$OUT_PWD/../udm-testing/ -ludm-testing

INCLUDEPATH += $$PWD/../udm-testing
DEPENDPATH += $$PWD/../udm-testing

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-common/ -lubuntu-download-manager-common

INCLUDEPATH += $$PWD/../ubuntu-download-manager-common
DEPENDPATH += $$PWD/../ubuntu-download-manager-common

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-priv/ -lubuntu-download-manager-priv

INCLUDEPATH += $$PWD/../ubuntu-download-manager-priv
DEPENDPATH += $$PWD/../ubuntu-download-manager-priv
