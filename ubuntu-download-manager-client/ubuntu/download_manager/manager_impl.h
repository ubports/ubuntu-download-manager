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
#include <ubuntu/download_manager/system/dbus_connection.h>
#include "download.h"
#include "error.h"
#include "group_download.h"
#include "manager_interface.h"
#include "manager_pendingcall_watcher.h"
#include "manager.h"


class QDBusConnection;

namespace Ubuntu {

namespace DownloadManager {

class Download;
class Error;
class GroupDownload;
class ManagerInterface;

class ManagerImpl : public Manager {
    Q_OBJECT

    // allow watchers to emit the signals
    friend class Manager;
    friend class DownloadManagerPendingCallWatcher;
    friend class GroupManagerPendingCallWatcher;

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
