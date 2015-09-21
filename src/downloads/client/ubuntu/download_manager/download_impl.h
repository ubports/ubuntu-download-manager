/*
 * Copyright 2013-2015 Canonical Ltd.
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

#pragma once

#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QObject>
#include <QVariantMap>
#include <QString>

#include <ubuntu/transfers/visibility.h>
#include <ubuntu/download_manager/metatypes.h>

#include <ubuntu/download_manager/download_interface.h>
#include <ubuntu/download_manager/download_pendingcall_watcher.h>
#include <ubuntu/download_manager/error.h>
#include <ubuntu/download_manager/properties_interface.h>

#include "download.h"

class QDBusConnection;
class QDBusObjectPath;

namespace Ubuntu {

namespace DownloadManager {

class Error;
class UBUNTU_TRANSFERS_PRIVATE DownloadImpl : public Download {
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
    void setHeaders(QMap<QString, QString> headers);
    QMap<QString, QString> headers();
    QVariantMap metadata();
    void setMetadata(QVariantMap map);
    void setThrottle(qulonglong speed);
    qulonglong throttle();

    QString id() const;
    qulonglong progress();
    qulonglong totalSize();

    bool isError() const;
    Error* error() const;

    QString clickPackage() const;
    bool showInIndicator() const;
    QString title() const;
    QString destinationApp() const;

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
    void onHashError(HashErrorStruct);
    void onPropertiesChanged(const QString& interfaceName,
                             const QVariantMap& changedProperties,
                             const QStringList& invalidatedProperties);

 private:
    QString _id;
    bool _isError = false;
    Error* _lastError = nullptr;
    DownloadInterface* _dbusInterface = nullptr;
    PropertiesInterface* _propertiesInterface = nullptr;
    QDBusConnection _conn;
    QString _servicePath;

};

}  // Ubuntu

}  // DownloadManager

