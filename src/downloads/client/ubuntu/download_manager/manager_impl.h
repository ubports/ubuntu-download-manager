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

#ifndef UBUNTU_DOWNLOADMANAGER_CLIENT_MANAGER_IMPL_H
#define UBUNTU_DOWNLOADMANAGER_CLIENT_MANAGER_IMPL_H

#include <QDBusConnection>
#include <QDBusObjectPath>

#include <ubuntu/transfers/visibility.h>
#include <ubuntu/transfers/system/dbus_connection.h>

#include <ubuntu/download_manager/download.h>
#include <ubuntu/download_manager/error.h>
#include <ubuntu/download_manager/group_download.h>
#include <ubuntu/download_manager/manager_interface.h>
#include <ubuntu/download_manager/manager_pendingcall_watcher.h>

#include "manager.h"


class QDBusConnection;

namespace Ubuntu {

namespace DownloadManager {

class Download;
class Error;
class GroupDownload;
class ManagerInterface;

class UBUNTU_TRANSFERS_PRIVATE ManagerImpl : public Manager {
    Q_OBJECT

    // allow watchers to emit the signals
    friend class Manager;
    friend class DownloadManagerPCW;
    friend class GroupManagerPCW;

 public:
    virtual ~ManagerImpl();
    virtual Download* getDownloadForId(const QString& id);
    virtual void createDownload(DownloadStruct downStruct);
    virtual void createDownload(DownloadStruct downStruct,
                                DownloadCb cb,
                                DownloadCb errCb);
    virtual void createDownload(StructList downs,
                                const QString &algorithm,
                                bool allowed3G,
                                const QVariantMap &metadata,
                                StringMap headers);
    virtual void createDownload(StructList downs,
                                const QString &algorithm,
                                bool allowed3G,
                                const QVariantMap &metadata,
                                StringMap headers,
                                GroupCb cb,
                                GroupCb errCb);
    virtual void getAllDownloads();
    virtual void getAllDownloads(DownloadsListCb cb,
                                 DownloadsListCb errCb);
    virtual void getAllDownloadsWithMetadata(const QString &name,
                                             const QString &value);
    virtual void getAllDownloadsWithMetadata(const QString &name,
                                             const QString &value,
                                             MetadataDownloadsListCb cb,
                                             MetadataDownloadsListCb errCb);

    bool isError() const;
    Error* lastError() const;
    void allowMobileDataDownload(bool allowed);
    bool isMobileDataDownload();
    qulonglong defaultThrottle();
    void setDefaultThrottle(qulonglong speed);
    void exit();

 protected:
    ManagerImpl(const QDBusConnection& conn,
            const QString& path = "",
            QObject* parent= 0);
    // used for testing purposes
    ManagerImpl(const QDBusConnection& conn,
            const QString& path,
            ManagerInterface* interface,
            QObject* parent);

 private:
    void init();
    void onWatcherDone();
    void setLastError(const QDBusError& err);

 private:
    bool _isError = false;
    QDBusConnection _conn;
    QString _servicePath;
    Error* _lastError = nullptr;
    ManagerInterface* _dbusInterface = nullptr;
};

}  // DownloadManager

}  // Ubuntu

#endif  // UBUNTU_DOWNLOADMANAGER_CLIENT_MANAGER_H
