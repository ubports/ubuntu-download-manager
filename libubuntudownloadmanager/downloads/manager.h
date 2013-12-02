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
#include <QSslCertificate>
#include <ubuntu/download_manager/metatypes.h>
#include "downloads/download.h"
#include "downloads/queue.h"
#include "downloads/factory.h"
#include "system/application.h"
#include "system/dbus_connection.h"

namespace Ubuntu {

namespace DownloadManager {

class Manager : public QObject, public QDBusContext {
    Q_OBJECT

 public:
    Manager(Application* app,
            DBusConnection* connection,
            bool stoppable = false,
            QObject *parent = 0);
    Manager(Application* app,
            DBusConnection* connection,
            Factory* downloadFactory,
            Queue* queue,
            bool stoppable = false,
            QObject *parent = 0);
    virtual ~Manager();

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

    typedef std::function<Download*(QString)> DownloadCreationFunc;

    void init();

    void loadPreviewsDownloads(QString path);
    void addDownload(Download* download);
    QDBusObjectPath registerDownload(Download* download);
    QDBusObjectPath createDownload(DownloadCreationFunc createDownloadFunc);
    QDBusObjectPath createDownload(const QString& url,
                                   const QString& hash,
                                   const QString& algo,
                                   const QVariantMap& metadata,
                                   StringMap headers);
    void onDownloadsChanged(QString);

 private:
    Application* _app;
    qulonglong _throttle;
    Factory* _downloadFactory;
    Queue* _downloadsQueue;
    DBusConnection* _conn;
    bool _stoppable;
};

}  // DownloadManager

}  // Manager
#endif  // DOWNLOADER_LIB_DOWNLOADER_H
