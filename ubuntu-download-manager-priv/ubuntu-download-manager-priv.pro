include(../common-project-config.pri)
include(../common-vars.pri)

QT       += network dbus systeminfo sql
QT       -= gui
DEFINES += NDEBUG

TARGET = ubuntu-download-manager-priv
TEMPLATE = lib

DEFINES += APPDOWNLOADERLIB_LIBRARY

SOURCES += \
    ubuntu/downloads/daemon.cpp \
    ubuntu/downloads/download.cpp \
    ubuntu/downloads/download_adaptor.cpp \
    ubuntu/downloads/downloads_db.cpp \
    ubuntu/downloads/factory.cpp \
    ubuntu/downloads/manager.cpp \
    ubuntu/downloads/download_manager_adaptor.cpp \
    ubuntu/downloads/queue.cpp \
    ubuntu/downloads/group_download.cpp \
    ubuntu/downloads/group_download_adaptor.cpp \
    ubuntu/downloads/file_download.cpp \
    ubuntu/downloads/sm_file_download.cpp \
    ubuntu/downloads/state_machines/download_sm.cpp \
    ubuntu/downloads/state_machines/state.cpp \
    ubuntu/downloads/state_machines/final_state.cpp \
    ubuntu/downloads/mms_file_download.cpp \
    ubuntu/downloads/download_adaptor_factory.cpp \
    ubuntu/downloads/download_manager_factory.cpp

HEADERS +=\
    ubuntu/downloads/daemon.h \
    ubuntu/downloads/download.h \
    ubuntu/downloads/download_adaptor.h \
    ubuntu/downloads/downloads_db.h \
    ubuntu/downloads/factory.h \
    ubuntu/downloads/manager.h \
    ubuntu/downloads/download_manager_adaptor.h \
    ubuntu/downloads/queue.h \
    ubuntu/downloads/group_download.h \
    ubuntu/downloads/group_download_adaptor.h \
    ubuntu/downloads/file_download.h \
    ubuntu/downloads/sm_file_download.h \
    ubuntu/downloads/state_machines/download_sm.h \
    ubuntu/downloads/state_machines/state.h \
    ubuntu/downloads/state_machines/final_state.h \
    ubuntu/downloads/mms_file_download.h \
    ubuntu/downloads/download_adaptor_factory.h \
    ubuntu/downloads/download_manager_factory.h

include(../common-installs-config.pri)

LIBS += -L$$OUT_PWD/../udm-common/ -ludm-common

INCLUDEPATH += $$PWD/../udm-common
DEPENDPATH += $$PWD/../udm-common

LIBS += -L$$OUT_PWD/../udm-priv-common/ -ludm-priv-common

INCLUDEPATH += $$PWD/../udm-priv-common
DEPENDPATH += $$PWD/../udm-priv-common

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-common/ -lubuntu-download-manager-common

INCLUDEPATH += $$PWD/../ubuntu-download-manager-common
DEPENDPATH += $$PWD/../ubuntu-download-manager-common
