/*
 * Copyright 2013-2014 Canonical Ltd.
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
#include <QSslCertificate>
#include <ubuntu/download_manager/metatypes.h>
#include <ubuntu/transfers/system/dbus_connection.h>
#include "ubuntu/transfers/base_manager.h"
#include "ubuntu/transfers/system/application.h"
#include "download.h"
#include "downloads_db.h"
#include "queue.h"
#include "factory.h"

namespace Ubuntu {

using namespace Transfers;

namespace DownloadManager {

namespace Daemon {

class DownloadManager : public BaseManager {
    Q_OBJECT

 public:
    DownloadManager(Application* app,
                    DBusConnection* connection,
                    bool stoppable = false,
                    QObject *parent = 0);
    DownloadManager(Application* app,
                    DBusConnection* connection,
                    Factory* downloadFactory,
                    Queue* queue,
                    bool stoppable = false,
                    QObject *parent = 0);
    virtual ~DownloadManager();

    void loadPreviewsDownloads(const QString &path);

    // mainly for testing purposes
    virtual QList<QSslCertificate> acceptedCertificates();
    virtual void setAcceptedCertificates(const QList<QSslCertificate>& certs);
    static const QString SERVICE_PATH;

 public slots:  // NOLINT(whitespace/indent)
    virtual QDBusObjectPath createDownload(DownloadStruct download);
    virtual QDBusObjectPath createMmsDownload(const QString& url,
                                              const QString& hostname,
                                              int port);

    virtual QDBusObjectPath createDownloadGroup(StructList downloads,
                                                const QString& algorithm,
                                                bool allowed3G,
                                                const QVariantMap& metadata,
                                                StringMap headers);

    virtual qulonglong defaultThrottle();
    virtual void setDefaultThrottle(qulonglong speed);
    virtual void allowGSMDownload(bool allowed);
    virtual bool isGSMDownloadAllowed();
    virtual QList<QDBusObjectPath> getAllDownloads();
    virtual QList<QDBusObjectPath> getAllDownloadsWithMetadata(
                                                      const QString& name,
                                                      const QString& value);
 signals:
    void downloadCreated(const QDBusObjectPath& path);

 protected:
    Queue* queue() {
        return _downloadsQueue;
    }

    virtual QDBusObjectPath registerDownload(Download* download);

 private:

    typedef std::function<Download*(QString)> DownloadCreationFunc;

    void init();

    void loadPreviewsDownloads(QString path);
    void addDownload(Download* download);
    QDBusObjectPath createDownload(DownloadCreationFunc createDownloadFunc);
    QDBusObjectPath createDownload(const QString& url,
                                   const QString& hash,
                                   const QString& algo,
                                   const QVariantMap& metadata,
                                   StringMap headers);
    void onDownloadsChanged(QString);

 private:
    Application* _app = nullptr;
    qulonglong _throttle;
    Factory* _downloadFactory = nullptr;
    Queue* _downloadsQueue = nullptr;
    DownloadsDb* _db = nullptr;
    DBusConnection* _conn = nullptr;
    bool _stoppable = false;
    bool _allowMobileData = true;
};

}  // Daemon

}  // DownloadManager

}  // Manager
#endif  // DOWNLOADER_LIB_DOWNLOADER_H
