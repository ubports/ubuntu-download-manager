#-------------------------------------------------
#
# Project created by QtCreator 2013-06-12T13:03:54
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ubuntu-download-manager
QMAKE_CXXFLAGS += -std=c++0x -Werror
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

LIBS += -L$$OUT_PWD/../libubuntudownloadmanager/ -lubuntudownloadmanager

INCLUDEPATH += $$PWD/../libubuntudownloadmanager
DEPENDPATH += $$PWD/../libubuntudownloadmanager

target.path = /usr/bin/ubuntu-download-manager
INSTALLS += target
