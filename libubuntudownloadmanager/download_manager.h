/*
 * Copyright 2013 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef DOWNLOADER_LIB_DOWNLOADER_H
#define DOWNLOADER_LIB_DOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QtDBus/QDBusObjectPath>
#include "dbus_connection.h"
#include "download.h"
#include "download_queue.h"
#include "uuid_factory.h"
#include "system_network_info.h"

class DownloadManagerPrivate;
class DownloadManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(DownloadManager)
public:
    explicit DownloadManager(DBusConnection* connection, QObject *parent = 0);
    explicit DownloadManager(DBusConnection* connection, SystemNetworkInfo* networkInfo, DownloadQueue* queue, UuidFactory* uuidFactory, QObject *parent = 0);
    void loadPreviewsDownloads(const QString &path);

public slots:
    QDBusObjectPath createDownload(const QString &url, const QVariantMap &metadata, const QVariantMap &headers);
    QDBusObjectPath createDownloadWithHash(const QString &url, const QString &algorithm, const QString &hash,
        const QVariantMap &metadata, const QVariantMap &headers);
    uint defaultThrottle();
    void setDefaultThrottle(uint speed);
    QList<QDBusObjectPath> getAllDownloads();
    QList<QDBusObjectPath> getAllDownloadsWithMetadata(const QString& name, const QString& value);

Q_SIGNALS:
    void downloadCreated(const QDBusObjectPath& path);

private:
    Q_PRIVATE_SLOT(d_func(), void onDownloadRemoved(QString))

private:
    // use pimpl so that we can mantains ABI compatibility
    DownloadManagerPrivate* d_ptr;
};

#endif // DOWNLOADER_H
