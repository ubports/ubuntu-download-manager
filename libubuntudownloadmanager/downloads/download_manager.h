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
#include <QtDBus/QDBusContext>
#include <QSharedPointer>
#include <QSslCertificate>
#include "metatypes.h"
#include "downloads/download.h"
#include "downloads/queue.h"
#include "downloads/factory.h"
#include "system/application.h"
#include "system/dbus_connection.h"
#include "system/system_network_info.h"

class DownloadManagerPrivate;
class DownloadManager : public QObject, public QDBusContext {
    Q_OBJECT
    Q_DECLARE_PRIVATE(DownloadManager)

 public:
    DownloadManager(QSharedPointer<Application> app,
                    QSharedPointer<DBusConnection> connection,
                    bool stoppable = false,
                    QObject *parent = 0);
    DownloadManager(QSharedPointer<Application> app,
                    QSharedPointer<DBusConnection> connection,
                    SystemNetworkInfo* networkInfo,
                    Factory* downloadFactory,
                    Queue* queue,
                    bool stoppable = false,
                    QObject *parent = 0);

    void loadPreviewsDownloads(const QString &path);

    // mainly for testing purposes
    virtual QList<QSslCertificate> acceptedCertificates();
    virtual void setAcceptedCertificates(const QList<QSslCertificate>& certs);

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
    void exit();

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
