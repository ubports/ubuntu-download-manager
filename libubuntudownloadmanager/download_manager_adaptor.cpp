/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c DownloadManagerAdaptor -a download_manager_adaptor.h:download_manager_adaptor.cpp -i metatypes.h com.canonical.applications.download_manager.xml
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "download_manager_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class DownloadManagerAdaptor
 */

DownloadManagerAdaptor::DownloadManagerAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

DownloadManagerAdaptor::~DownloadManagerAdaptor()
{
    // destructor
}

void DownloadManagerAdaptor::allowGSMDownload(bool allowed)
{
    // handle method call com.canonical.applications.DownloadManager.allowGSMDownload
    QMetaObject::invokeMethod(parent(), "allowGSMDownload", Q_ARG(bool, allowed));
}

QDBusObjectPath DownloadManagerAdaptor::createDownload(DownloadStruct download)
{
    // handle method call com.canonical.applications.DownloadManager.createDownload
    QDBusObjectPath downloadPath;
    QMetaObject::invokeMethod(parent(), "createDownload", Q_RETURN_ARG(QDBusObjectPath, downloadPath), Q_ARG(DownloadStruct, download));
    return downloadPath;
}

QDBusObjectPath DownloadManagerAdaptor::createDownloadGroup(StructList downloads, const QString &algorithm, bool allowed3G, const QVariantMap &metadata, StringMap headers)
{
    // handle method call com.canonical.applications.DownloadManager.createDownloadGroup
    QDBusObjectPath download;
    QMetaObject::invokeMethod(parent(), "createDownloadGroup", Q_RETURN_ARG(QDBusObjectPath, download), Q_ARG(StructList, downloads), Q_ARG(QString, algorithm), Q_ARG(bool, allowed3G), Q_ARG(QVariantMap, metadata), Q_ARG(StringMap, headers));
    return download;
}

qulonglong DownloadManagerAdaptor::defaultThrottle()
{
    // handle method call com.canonical.applications.DownloadManager.defaultThrottle
    qulonglong speed;
    QMetaObject::invokeMethod(parent(), "defaultThrottle", Q_RETURN_ARG(qulonglong, speed));
    return speed;
}

QList<QDBusObjectPath> DownloadManagerAdaptor::getAllDownloads()
{
    // handle method call com.canonical.applications.DownloadManager.getAllDownloads
    QList<QDBusObjectPath> downloads;
    QMetaObject::invokeMethod(parent(), "getAllDownloads", Q_RETURN_ARG(QList<QDBusObjectPath>, downloads));
    return downloads;
}

QList<QDBusObjectPath> DownloadManagerAdaptor::getAllDownloadsWithMetadata(const QString &name, const QString &value)
{
    // handle method call com.canonical.applications.DownloadManager.getAllDownloadsWithMetadata
    QList<QDBusObjectPath> downloads;
    QMetaObject::invokeMethod(parent(), "getAllDownloadsWithMetadata", Q_RETURN_ARG(QList<QDBusObjectPath>, downloads), Q_ARG(QString, name), Q_ARG(QString, value));
    return downloads;
}

bool DownloadManagerAdaptor::isGSMDownloadAllowed()
{
    // handle method call com.canonical.applications.DownloadManager.isGSMDownloadAllowed
    bool allowed;
    QMetaObject::invokeMethod(parent(), "isGSMDownloadAllowed", Q_RETURN_ARG(bool, allowed));
    return allowed;
}

void DownloadManagerAdaptor::setDefaultThrottle(qulonglong speed)
{
    // handle method call com.canonical.applications.DownloadManager.setDefaultThrottle
    QMetaObject::invokeMethod(parent(), "setDefaultThrottle", Q_ARG(qulonglong, speed));
}

