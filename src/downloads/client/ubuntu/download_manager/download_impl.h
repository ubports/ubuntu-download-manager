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

#ifndef UBUNTU_DOWNLOADMANAGER_CLIENT_DOWNLOAD_IMPL_H
#define UBUNTU_DOWNLOADMANAGER_CLIENT_DOWNLOAD_IMPL_H

#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QObject>
#include <QVariantMap>
#include <QString>
#include <ubuntu/download_manager/common.h>
#include <ubuntu/download_manager/metatypes.h>
#include "download_interface.h"
#include "download_pendingcall_watcher.h"
#include "error.h"
#include "download.h"

class QDBusConnection;
class QDBusObjectPath;

namespace Ubuntu {

namespace DownloadManager {

class Error;
class DownloadImpl : public Download {
    Q_OBJECT

    // allow the manager to create downloads
    friend class ManagerImpl;
    friend class DownloadPCW;
    friend class MetadataDownloadsListManagerPCW;
    friend class DownloadsListManagerPCW;
    friend class DownloadManagerPCW;

 public:
    virtual ~DownloadImpl();

    void start();
    void pause();
    void resume();
    void cancel();

    void allowMobileDownload(bool allowed);
    bool isMobileDownloadAllowed();

    void setDestinationDir(const QString& path);
    void setThrottle(qulonglong speed);
    qulonglong throttle();

    QString id() const;
    QVariantMap metadata();
    qulonglong progress();
    qulonglong totalSize();

    bool isError() const;
    Error* error() const;

 protected:
    DownloadImpl(const QDBusConnection& conn, Error* err, QObject* parent = 0);
    DownloadImpl(const QDBusConnection& conn,
                 const QString& servicePath,
                 const QDBusObjectPath& objectPath,
                 QObject* parent = 0);

 private:
    void setLastError(Error* err);
    void setLastError(const QDBusError& err);
    void onHttpError(HttpErrorStruct);
    void onNetworkError(NetworkErrorStruct);
    void onProcessError(ProcessErrorStruct);
    void onAuthError(AuthErrorStruct);

 private:
    QString _id;
    bool _isError = false;
    Error* _lastError = nullptr;
    DownloadInterface* _dbusInterface = nullptr;
    QDBusConnection _conn;
    QString _servicePath;

};

}  // Ubuntu

}  // DownloadManager

#endif  // UBUNTU_DOWNLOADMANAGER_CLIENT_DOWNLOAD_IMPL_H
