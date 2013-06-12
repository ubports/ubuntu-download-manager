/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c DownloaderAdaptor -a /home/mandel/Canonical/junk/app-downloader/app-downloader/app-downloader-lib/downloader_adaptor.h:/home/mandel/Canonical/junk/app-downloader/app-downloader/app-downloader-lib/downloader_adaptor.cpp /home/mandel/Canonical/junk/app-downloader/app-downloader/app-downloader-lib/com.canonical.applications.downloader.xml
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "/home/mandel/Canonical/junk/app-downloader/app-downloader/app-downloader-lib/downloader_adaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class DownloaderAdaptor
 */

DownloaderAdaptor::DownloaderAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

DownloaderAdaptor::~DownloaderAdaptor()
{
    // destructor
}

QDBusObjectPath DownloaderAdaptor::createDownload(const QString &url)
{
    // handle method call com.canonical.applications.Downloader.createDownload
    QDBusObjectPath download;
    QMetaObject::invokeMethod(parent(), "createDownload", Q_RETURN_ARG(QDBusObjectPath, download), Q_ARG(QString, url));
    return download;
}

