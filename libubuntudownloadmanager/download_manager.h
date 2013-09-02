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
#include <QSharedPointer>
#include "./dbus_connection.h"
#include "./download.h"
#include "./download_queue.h"
#include "./download_factory.h"
#include "./metatypes.h"
#include "./system_network_info.h"

class DownloadManagerPrivate;
class DownloadManager : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(DownloadManager)

 public:
    DownloadManager(QSharedPointer<DBusConnection> connection,
                    QObject *parent = 0);
    DownloadManager(QSharedPointer<DBusConnection> connection,
                    SystemNetworkInfo* networkInfo,
                    DownloadFactory* downloadFactory,
                    DownloadQueue* queue,
                    QObject *parent = 0);
    void loadPreviewsDownloads(const QString &path);

 public slots:  // NOLINT(whitespace/indent)
    QDBusObjectPath createDownload(DownloadStruct download);

    QDBusObjectPath createDownloadGroup(StructList downloads,
                                        const QString& algorithm,
                                        bool allowed3G,
                                        const QVariantMap& metadata,
                                        StringMap headers);

    qulonglong defaultThrottle();
    void setDefaultThrottle(qulonglong speed);
    QList<QDBusObjectPath> getAllDownloads();
    QList<QDBusObjectPath> getAllDownloadsWithMetadata(const QString& name,
                                                       const QString& value);

 signals:
    void downloadCreated(const QDBusObjectPath& path);
    void sizeChanged(int count);

 private:
    Q_PRIVATE_SLOT(d_func(), void onDownloadsChanged(QString))

 private:
    // use pimpl so that we can mantains ABI compatibility
    DownloadManagerPrivate* d_ptr;
};

#endif  // DOWNLOADER_LIB_DOWNLOADER_H
