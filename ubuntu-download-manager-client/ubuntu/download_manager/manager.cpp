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

#include <QDebug>
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
    ManagerPrivate(QDBusConnection conn, const QString& path, Manager* parent)
        : q_ptr(parent) {
        init();
        _dbusInterface = new ManagerInterface(path, MANAGER_PATH,
            conn);
    }

    // used for testing purposes
    ManagerPrivate(ManagerInterface* interface, Manager* parent)
        : _dbusInterface(interface),
          q_ptr(parent) {
        init();
    }

    ~ManagerPrivate() {
        delete _dbusInterface;
    }

    void init() {
        qRegisterMetaType<Download*>("Download*");
        qRegisterMetaType<GroupDownload*>("GroupDownload*");
        qRegisterMetaType<Error*>();
        qDBusRegisterMetaType<StringMap>();
        qDBusRegisterMetaType<DownloadStruct>();
        qDBusRegisterMetaType<GroupDownloadStruct>();
        qDBusRegisterMetaType<StructList>();
        qDBusRegisterMetaType<HttpErrorStruct>();
        qDBusRegisterMetaType<NetworkErrorStruct>();
        qDBusRegisterMetaType<ProcessErrorStruct>();
    }

    Download* createDownload(DownloadStruct downStruct) {
        Q_Q(Manager);
        QDBusPendingReply<QDBusObjectPath> reply =
            _dbusInterface->createDownload(downStruct);
        // blocking other method should be used
        reply.waitForFinished();
        if (reply.isError()) {
            auto err = new Error(reply.error());
            return new Download(err);
        } else {
            auto path = reply.value();
            auto down = new Download(path, q);
            emit q->downloadCreated(down);
            return down;
        }
    }

    void createDownload(DownloadStruct downStruct,
                        DownloadCb cb,
                        DownloadCb errCb) {
        Q_Q(Manager);
        QDBusPendingCall call =
            _dbusInterface->createDownload(downStruct);
        auto watcher = new DownloadManagerPendingCallWatcher(call, cb, errCb,
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
            auto path = reply.value();
            auto down = new GroupDownload(path, q);
            emit q->groupCreated(down);
            return down;
        }
    }

    void createDownload(StructList downs,
                        const QString& algorithm,
                        bool allowed3G,
                        const QVariantMap& metadata,
                        StringMap headers,
                        GroupCb cb,
                        GroupCb errCb) {
        Q_Q(Manager);
        QDBusPendingCall call =
            _dbusInterface->createDownloadGroup(downs,
                algorithm, allowed3G, metadata, headers);
        auto watcher = new GroupManagerPendingCallWatcher(call, cb, errCb,
                static_cast<QObject*>(q));
        q->connect(watcher, SIGNAL(callbackExecuted()),
            q, SLOT(onWatcherDone()));
    }

    bool isError() {
        return _isError;
    }

    Error* lastError() {
        return _lastError;
    }

    void setLastError(const QDBusError& err) {
        Q_Q(Manager);
        // delete the last if error if present to keep mem to a minimum
        if (_lastError != NULL) {
            delete _lastError;
        }
        _lastError = new Error(err, q);
        _isError = true;
    }

    void allowMobileDataDownload(bool allowed) {
        QDBusPendingReply<> reply =
            _dbusInterface->allowGSMDownload(allowed);
        // we block but because we expect it to be fast
        reply.waitForFinished();
        if (reply.isError()) {
            auto err = reply.error();
            qCritical() << "Error setting mobile data" << err;
            setLastError(err);
        }
    }

    bool isMobileDataDownload() {
        QDBusPendingReply<bool> reply =
            _dbusInterface->isGSMDownloadAllowed();
        // we block but because we expect it to be fast
        reply.waitForFinished();
        if (reply.isError()) {
            auto err = reply.error();
            qCritical() << "Error getting if mobile data is enabled"
                << err;
            setLastError(err);
            return false;
        } else {
            return reply.value();
        }
    }

    qulonglong defaultThrottle() {
        QDBusPendingReply<qulonglong> reply =
            _dbusInterface->defaultThrottle();
        // we block but because we expect it to be fast
        reply.waitForFinished();
        if (reply.isError()) {
            auto err = reply.error();
            qCritical() << "Error getting the default throttle" << err;
            setLastError(err);
            return 0;
        } else {
            return reply.value();
        }
    }

    void setDefaultThrottle(qulonglong speed) {
        QDBusPendingReply<> reply =
            _dbusInterface->setDefaultThrottle(speed);
        // we block but because we expect it to be fast
        reply.waitForFinished();
        if (reply.isError()) {
            auto err = reply.error();
            qCritical() << "Error setting default throttle" << err;
            setLastError(err);
        }
    }

    void exit() {
        QDBusPendingReply<> reply =
            _dbusInterface->exit();
        // we block but because we expect it to be fast
        reply.waitForFinished();
        if (reply.isError()) {
            auto err = reply.error();
            qCritical() << "Error setting killing the daemon" << err;
            setLastError(err);
        }
    }

    void onWatcherDone() {
        Q_Q(Manager);
        auto senderObj = q->sender();
        senderObj->deleteLater();
    }

 private:
    bool _isError = false;
    Error* _lastError = NULL;
    ManagerInterface* _dbusInterface;
    Manager* q_ptr;
};

/*
 * PUBLIC IMPLEMENTATION
 */

Manager::Manager(QDBusConnection conn, const QString& path, QObject* parent)
    : QObject(parent),
      d_ptr(new ManagerPrivate(conn, path, this)){
}

Manager::Manager(ManagerInterface* interface, QObject* parent)
    : QObject(parent),
      d_ptr(new ManagerPrivate(interface, this)) {
}

Manager::~Manager() {
    delete d_ptr;
}

Manager*
Manager::createSessionManager(const QString& path, QObject* parent) {
    if (path.isEmpty()) {
        return new Manager(QDBusConnection::sessionBus(), DOWNLOAD_SERVICE, parent);
    } else {
        return new Manager(QDBusConnection::sessionBus(), path, parent);
    }
}

Manager*
Manager::createSystemManager(const QString& path, QObject* parent) {
    if (path.isEmpty()) {
        return new Manager(QDBusConnection::systemBus(), DOWNLOAD_SERVICE, parent);
    } else {
        return new Manager(QDBusConnection::systemBus(), path, parent);
    }
}

Download*
Manager::createDownload(DownloadStruct downStruct) {
    Q_D(Manager);
    return d->createDownload(downStruct);
}

void
Manager::createDownload(DownloadStruct downStruct,
                        DownloadCb cb,
                        DownloadCb errCb) {
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
                        GroupCb cb,
                        GroupCb errCb) {
    Q_D(Manager);
    d->createDownload(downs, algorithm, allowed3G, metadata, headers, cb,
        errCb);
}

bool
Manager::isError() {
    Q_D(Manager);
    return d->isError();
}

Error*
Manager::lastError() {
    Q_D(Manager);
    return d->lastError();
}

void
Manager::allowMobileDataDownload(bool allowed) {
    Q_D(Manager);
    d->allowMobileDataDownload(allowed);
}

bool
Manager::isMobileDataDownload() {
    Q_D(Manager);
    return d->isMobileDataDownload();
}

qulonglong
Manager::defaultThrottle() {
    Q_D(Manager);
    return d->defaultThrottle();
}

void
Manager::setDefaultThrottle(qulonglong speed) {
    Q_D(Manager);
    d->setDefaultThrottle(speed);
}

void
Manager::exit() {
    Q_D(Manager);
    d->exit();
}

}  // DownloadManager

}  // Ubuntu

#include "moc_manager.cpp"
