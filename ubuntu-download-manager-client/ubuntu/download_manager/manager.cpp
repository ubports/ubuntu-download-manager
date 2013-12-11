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

#include <QDBusObjectPath>
#include <ubuntu/download_manager/system/dbus_connection.h>
#include "download.h"
#include "error.h"
#include "group_download.h"
#include "manager_interface.h"
#include "manager_pendingcall_watcher.h"
#include "manager.h"


#define DOWNLOAD_SERVICE "com.canonical.applications.Downloader"
#define MANAGER_PATH "/"


namespace Ubuntu {

namespace DownloadManager {

/*
 * PRIVATE IMPLEMENTATION
 */

class ManagerPrivate {
    Q_DECLARE_PUBLIC(Manager)

 public:
    ManagerPrivate(QDBusConnection conn, Manager* parent)
        : q_ptr(parent) {
        _dbusInterface = new ManagerInterface(DOWNLOAD_SERVICE, MANAGER_PATH,
            conn);
    }

    // used for testing purposes
    ManagerPrivate(ManagerInterface* interface, Manager* parent)
        : _dbusInterface(interface),
          q_ptr(parent) {
    }

    ~ManagerPrivate() {
        delete _dbusInterface;
    }

    Download* createDownload(DownloadStruct downStruct) {
        Q_Q(Manager);
        QDBusPendingReply<QDBusObjectPath> reply =
            _dbusInterface->createDownload(downStruct);
        // blocking other method should be used
        reply.waitForFinished();
        if (reply.isError()) {
            Error* err = new Error(reply.error());
            return new Download(err);
        } else {
            QDBusObjectPath path = reply.value();
            return new Download(path, q);
        }
    }

    void createDownload(DownloadStruct downStruct,
                        DownloadCreationCb cb,
                        ErrorCb errCb) {
        Q_Q(Manager);
        QDBusPendingCall call =
            _dbusInterface->createDownload(downStruct);
        DownloadManagerPendingCallWatcher* watcher =
            new DownloadManagerPendingCallWatcher(call, cb, errCb,
                static_cast<QObject*>(q));
        q->connect(watcher, SIGNAL(callbackExecuted()),
            q, SLOT(onWatcherDone()));
    }

    GroupDownload* createDownload(StructList downs,
                                  const QString& algorithm,
                                  bool allowed3G,
                                  const QVariantMap& metadata,
                                  StringMap headers) {
        Q_Q(Manager);
        QDBusPendingReply<QDBusObjectPath> reply =
            _dbusInterface->createDownloadGroup(downs,
                algorithm, allowed3G, metadata, headers);
        // blocking other method should be used
        reply.waitForFinished();
        if (reply.isError()) {
            Error* err = new Error(reply.error());
            return new GroupDownload(err);
        } else {
            QDBusObjectPath path = reply.value();
            return new GroupDownload(path, q);
        }
    }

    void createDownload(StructList downs,
                        const QString& algorithm,
                        bool allowed3G,
                        const QVariantMap& metadata,
                        StringMap headers,
                        GroupCreationCb cb,
                        ErrorCb errCb) {
        Q_Q(Manager);
        QDBusPendingCall call =
            _dbusInterface->createDownloadGroup(downs,
                algorithm, allowed3G, metadata, headers);
        GroupManagerPendingCallWatcher* watcher =
            new GroupManagerPendingCallWatcher(call, cb, errCb,
                static_cast<QObject*>(q));
        q->connect(watcher, SIGNAL(callbackExecuted()),
            q, SLOT(onWatcherDone()));
    }

    void onWatcherDone() {
        Q_Q(Manager);
        QObject* senderObj = q->sender();
        senderObj->deleteLater();
    }

 private:
    ManagerInterface* _dbusInterface;
    Manager* q_ptr;
};

/*
 * PUBLIC IMPLEMENTATION
 */

Manager::Manager(QDBusConnection conn, QObject* parent)
    : QObject(parent),
      d_ptr(new ManagerPrivate(conn, this)){
}

Manager::Manager(ManagerInterface* interface, QObject* parent)
    : QObject(parent),
      d_ptr(new ManagerPrivate(interface, this)) {
}

Manager::~Manager() {
    delete d_ptr;
}

Manager*
Manager::createSessionManager(QObject* parent) {
    return new Manager(QDBusConnection::sessionBus(), parent);
}

Manager*
Manager::createSystemManager(QObject* parent) {
    return new Manager(QDBusConnection::systemBus(), parent);
}

Download*
Manager::createDownload(DownloadStruct downStruct) {
    Q_D(Manager);
    return d->createDownload(downStruct);
}

void
Manager::createDownload(DownloadStruct downStruct,
                        DownloadCreationCb cb,
                        ErrorCb errCb) {
    Q_D(Manager);
    d->createDownload(downStruct, cb, errCb);
}

GroupDownload*
Manager::createDownload(StructList downs,
                        const QString &algorithm,
                        bool allowed3G,
                        const QVariantMap &metadata,
                        StringMap headers) {
    Q_D(Manager);
    return d->createDownload(downs, algorithm, allowed3G, metadata, headers);
}

void
Manager::createDownload(StructList downs,
                        const QString& algorithm,
                        bool allowed3G,
                        const QVariantMap& metadata,
                        StringMap headers,
                        GroupCreationCb cb,
                        ErrorCb errCb) {
    Q_D(Manager);
    d->createDownload(downs, algorithm, allowed3G, metadata, headers, cb,
        errCb);
}

}  // DownloadManager

}  // Ubuntu

#include "moc_manager.cpp"
