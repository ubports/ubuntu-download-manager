/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c TestingManagerAdaptor -a testing_manager_adaptor.h:testing_manager_adaptor.cpp com.canonical.applications.testing.download_manager.xml
 *
 * qdbusxml2cpp is Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "testing_manager_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#include <QDebug>

/*
 * Implementation of adaptor class TestingManagerAdaptor
 */

TestingManagerAdaptor::TestingManagerAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

TestingManagerAdaptor::~TestingManagerAdaptor()
{
    // destructor
}

void TestingManagerAdaptor::returnAuthError(const QString &download, AuthErrorStruct error)
{
    // handle method call com.canonical.applications.testing.DownloadManager.returnAuthError
    QMetaObject::invokeMethod(parent(), "returnAuthError", Q_ARG(QString, download), Q_ARG(AuthErrorStruct, error));
}

void TestingManagerAdaptor::returnDBusErrors(bool returnErrors)
{
    // handle method call com.canonical.applications.testing.DownloadManager.returnDBusErrors
    QMetaObject::invokeMethod(parent(), "returnDBusErrors", Q_ARG(bool, returnErrors));
}

void TestingManagerAdaptor::returnHashError(const QString &download, HashErrorStruct error)
{
    // handle method call com.canonical.applications.testing.DownloadManager.returnHashError
    QMetaObject::invokeMethod(parent(), "returnHashError", Q_ARG(QString, download), Q_ARG(HashErrorStruct, error));
}

void TestingManagerAdaptor::returnHttpError(const QString &download, HttpErrorStruct error)
{
    // handle method call com.canonical.applications.testing.DownloadManager.returnHttpError
    QMetaObject::invokeMethod(parent(), "returnHttpError", Q_ARG(QString, download), Q_ARG(HttpErrorStruct, error));
}

void TestingManagerAdaptor::returnNetworkError(const QString &download, NetworkErrorStruct error)
{
    // handle method call com.canonical.applications.testing.DownloadManager.returnNetworkError
    QMetaObject::invokeMethod(parent(), "returnNetworkError", Q_ARG(QString, download), Q_ARG(NetworkErrorStruct, error));
}

void TestingManagerAdaptor::returnProcessError(const QString &download, ProcessErrorStruct error)
{
    // handle method call com.canonical.applications.testing.DownloadManager.returnProcessError
    QMetaObject::invokeMethod(parent(), "returnProcessError", Q_ARG(QString, download), Q_ARG(ProcessErrorStruct, error));
}

