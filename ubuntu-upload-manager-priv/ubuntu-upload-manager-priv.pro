include(../common-project-config.pri)
include(../common-vars.pri)

QT       += network dbus systeminfo sql
QT       -= gui
DEFINES += NDEBUG

TARGET = ubuntu-upload-manager-priv
TEMPLATE = lib

DEFINES += UBUNTUUPLOADMANAGERPRIV_LIBRARY

SOURCES += \
    ubuntu/uploads/daemon.cpp \
    ubuntu/uploads/manager.cpp \
    ubuntu/uploads/factory.cpp \
    ubuntu/uploads/file_upload.cpp \
    ubuntu/uploads/mms_file_upload.cpp \
    ubuntu/uploads/upload_manager_factory.cpp \
    ubuntu/uploads/upload_adaptor_factory.cpp \
    ubuntu/uploads/upload_adaptor.cpp \
    ubuntu/uploads/upload_manager_adaptor.cpp

HEADERS += \
    ubuntu/uploads/daemon.h \
    ubuntu/uploads/manager.h \
    ubuntu/uploads/factory.h \
    ubuntu/uploads/file_upload.h \
    ubuntu/uploads/mms_file_upload.h \
    ubuntu/uploads/upload_manager_factory.h \
    ubuntu/uploads/upload_adaptor_factory.h \
    ubuntu/uploads/upload_adaptor.h \
    ubuntu/uploads/upload_manager_adaptor.h

include(../common-installs-config.pri)

LIBS += -L$$OUT_PWD/../udm-common/ -ludm-common

INCLUDEPATH += $$PWD/../udm-common
DEPENDPATH += $$PWD/../udm-common

LIBS += -L$$OUT_PWD/../ubuntu-upload-manager-common/ -lubuntu-upload-manager-common

INCLUDEPATH += $$PWD/../ubuntu-upload-manager-common
DEPENDPATH += $$PWD/../ubuntu-upload-manager-common

LIBS += -L$$OUT_PWD/../ubuntu-upload-manager-common/ -lubuntu-upload-manager-common

INCLUDEPATH += $$PWD/../ubuntu-upload-manager-common
DEPENDPATH += $$PWD/../ubuntu-upload-manager-common

LIBS += -L$$OUT_PWD/../ubuntu-download-manager-common/ -lubuntu-download-manager-common

INCLUDEPATH += $$PWD/../ubuntu-download-manager-common
DEPENDPATH += $$PWD/../ubuntu-download-manager-common

LIBS += -L$$OUT_PWD/../udm-priv-common/ -ludm-priv-common

INCLUDEPATH += $$PWD/../udm-priv-common
DEPENDPATH += $$PWD/../udm-priv-common
