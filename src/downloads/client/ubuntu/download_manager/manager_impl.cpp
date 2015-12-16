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

#include <ubuntu/download_manager/download_impl.h>
#include <ubuntu/download_manager/downloads_list.h>
#include <ubuntu/download_manager/logging/logger.h>

#include "manager_impl.h"


namespace {
    const QString MANAGER_PATH = "/";
}

namespace Ubuntu {

namespace DownloadManager {

using namespace Logging;

ManagerImpl::ManagerImpl(const QDBusConnection& conn,
                         const QString& path,
                         QObject* parent)
    : Manager(parent),
      _conn(conn),
      _servicePath(path) {
    _dbusInterface = new ManagerInterface(path, MANAGER_PATH, conn);
    init();
}

// used for testing purposes
ManagerImpl::ManagerImpl(const QDBusConnection& conn,
                         const QString& path,
                         ManagerInterface* interface,
                         QObject* parent)
    : Manager(parent),
      _conn(conn),
      _servicePath(path),
      _dbusInterface(interface) {
    init();
}

ManagerImpl::~ManagerImpl() {
    delete _lastError;
    delete _dbusInterface;
}

void
ManagerImpl::init() {
    qRegisterMetaType<Download*>("Download*");
    qRegisterMetaType<GroupDownload*>("GroupDownload*");
    qRegisterMetaType<Error*>("Error*");
    qRegisterMetaType<DBusError*>("DBusError*");
    qRegisterMetaType<HttpError*>("HttpError*");
    qRegisterMetaType<NetworkError*>("NetworkError*");
    qRegisterMetaType<AuthError*>("AuthError*");
    qRegisterMetaType<HashError*>("HashError*");
    qRegisterMetaType<ProcessError*>("ProcessError*");
    qRegisterMetaType<DownloadsList*>("DownloadsList*");
    qDBusRegisterMetaType<StringMap>();
    qDBusRegisterMetaType<DownloadStruct>();
    qDBusRegisterMetaType<GroupDownloadStruct>();
    qDBusRegisterMetaType<StructList>();
    qDBusRegisterMetaType<AuthErrorStruct>();
    qDBusRegisterMetaType<HashErrorStruct>();
    qDBusRegisterMetaType<HttpErrorStruct>();
    qDBusRegisterMetaType<NetworkErrorStruct>();
    qDBusRegisterMetaType<ProcessErrorStruct>();
}

Download*
ManagerImpl::getDownloadForId(const QString& id) {
    Logger::log(Logger::Debug, QString("Manager getDownloadForId(%1)").arg(id));
    auto down = new DownloadImpl(_conn, _servicePath, QDBusObjectPath(id));
    return down;
}

void
ManagerImpl::createDownload(DownloadStruct downStruct) {
    Logger::log(Logger::Debug, "Manager createDownload(%1)", downStruct);
    DownloadCb cb = [](Download*) {};
    createDownload(downStruct, cb, cb);
}

void
ManagerImpl::createDownload(DownloadStruct downStruct,
                    DownloadCb cb,
                    DownloadCb errCb) {
    QDBusPendingCall call =
        _dbusInterface->createDownload(downStruct);
    auto watcher = new DownloadManagerPCW(_conn,
            _servicePath, call, cb, errCb, this);
    auto connected = connect(watcher, &DownloadManagerPCW::callbackExecuted,
        this, &ManagerImpl::onWatcherDone);
    if (!connected) {
        Logger::log(Logger::Critical,
            "Could not connect to signal &DownloadManagerPCW::callbackExecuted,");
    }
}

void
ManagerImpl::createDownload(StructList downs,
                    const QString& algorithm,
                    bool allowed3G,
                    const QVariantMap& metadata,
                    StringMap headers) {
    GroupCb cb = [](GroupDownload*) {};
    createDownload(downs, algorithm, allowed3G, metadata, headers, cb, cb);
}

void
ManagerImpl::createDownload(StructList downs,
                    const QString& algorithm,
                    bool allowed3G,
                    const QVariantMap& metadata,
                    StringMap headers,
                    GroupCb cb,
                    GroupCb errCb) {
    QDBusPendingCall call =
        _dbusInterface->createDownloadGroup(downs,
            algorithm, allowed3G, metadata, headers);
    auto watcher = new GroupManagerPCW(_conn, _servicePath,
            call, cb, errCb, this);
    auto connected = connect(watcher, &GroupManagerPCW::callbackExecuted,
        this, &ManagerImpl::onWatcherDone);
    if (!connected) {
        Logger::log(Logger::Critical,
            "Could not connect to signal &GroupManagerPCW::callbackExecuted");
    }
}

void
ManagerImpl::getAllDownloads(const QString &appId, bool uncollected) {
    Logger::log(Logger::Debug, QString("Manager getAllDownloads(%1, %2)").arg(appId).arg(uncollected));
    DownloadsListCb cb = [](DownloadsList*){};
    getAllDownloads(appId, uncollected, cb, cb);
}

void
ManagerImpl::getAllDownloads(const QString &appId, bool uncollected, DownloadsListCb cb, DownloadsListCb errCb) {
    Logger::log(Logger::Debug, QString("Manager getAllDownloads(%1, %2)").arg(appId).arg(uncollected));
    QDBusPendingCall call = _dbusInterface->getAllDownloads(appId, uncollected);
    auto watcher = new DownloadsListManagerPCW(
        _conn, _servicePath, call, cb, errCb, this);
    auto connected = connect(watcher, &GroupManagerPCW::callbackExecuted,
        this, &ManagerImpl::onWatcherDone);
    if (!connected) {
        Logger::log(Logger::Critical,
            "Could not connect to signal");
    }
}

void
ManagerImpl::getAllDownloadsWithMetadata(const QString &name,
                                         const QString &value) {
    Logger::log(Logger::Debug,
        QString("Manager getAllDownloadsWithMetadata(%1, %2)").arg(name).arg(value));
    MetadataDownloadsListCb cb =
        [](const QString&, const QString&, DownloadsList*){};
    getAllDownloadsWithMetadata(name, value, cb, cb);
}

void
ManagerImpl::getAllDownloadsWithMetadata(const QString &name,
                                         const QString &value,
                                         MetadataDownloadsListCb cb,
                                         MetadataDownloadsListCb errCb) {
    Logger::log(Logger::Debug,
        QString("Manager getAllDownloadsWithMetadata(%1, %2)").arg(name).arg(value));
    QDBusPendingCall call = _dbusInterface->getAllDownloadsWithMetadata(
    name, value);
    auto watcher = new MetadataDownloadsListManagerPCW(
        _conn, _servicePath, call, name, value, cb, errCb, this);
    auto connected = connect(watcher, &GroupManagerPCW::callbackExecuted,
        this, &ManagerImpl::onWatcherDone);
    if (!connected) {
        Logger::log(Logger::Critical, "Could not connect to signal");
    }
}

bool
ManagerImpl::isError() const {
    return _isError;
}

Error*
ManagerImpl::lastError() const {
    return _lastError;
}

void
ManagerImpl::setLastError(const QDBusError& err) {
    // delete the last if error if present to keep mem to a minimum
    if (_lastError != nullptr) {
        delete _lastError;
    }
    _lastError = new DBusError(err);
    _isError = true;
}

void
ManagerImpl::allowMobileDataDownload(bool allowed) {
    Logger::log(Logger::Debug,
        QString("Manager allowMobileDataDownload(%1)").arg(allowed));
    QDBusPendingReply<> reply =
        _dbusInterface->allowGSMDownload(allowed);
    // we block but because we expect it to be fast
    reply.waitForFinished();
    if (reply.isError()) {
        auto err = reply.error();
        Logger::log(Logger::Critical, "Error setting mobile data");
        setLastError(err);
    }
}

bool
ManagerImpl::isMobileDataDownload() {
    Logger::log(Logger::Debug, "Manager isMobileDataDownload()");
    QDBusPendingReply<bool> reply =
        _dbusInterface->isGSMDownloadAllowed();
    // we block but because we expect it to be fast
    reply.waitForFinished();
    if (reply.isError()) {
        auto err = reply.error();
        Logger::log(Logger::Error, "Error getting if mobile data is enabled");
        setLastError(err);
        return false;
    } else {
        return reply.value();
    }
}

qulonglong
ManagerImpl::defaultThrottle() {
    Logger::log(Logger::Debug, "Manager defaultThrottle()");
    QDBusPendingReply<qulonglong> reply =
        _dbusInterface->defaultThrottle();
    // we block but because we expect it to be fast
    reply.waitForFinished();
    if (reply.isError()) {
        auto err = reply.error();
        Logger::log(Logger::Error, "Error getting the default throttle");
        setLastError(err);
        return 0;
    } else {
        return reply.value();
    }
}

void
ManagerImpl::setDefaultThrottle(qulonglong speed) {
    Logger::log(Logger::Debug,
        QString("Manager setDefaultThrottle(%1)").arg(speed));
    QDBusPendingReply<> reply =
        _dbusInterface->setDefaultThrottle(speed);
    // we block but because we expect it to be fast
    reply.waitForFinished();
    if (reply.isError()) {
        auto err = reply.error();
        Logger::log(Logger::Error, "Error setting default throttle");
        setLastError(err);
    }
}

void
ManagerImpl::exit() {
    Logger::log(Logger::Debug, "Manager exit()");
    QDBusPendingReply<> reply =
        _dbusInterface->exit();
    // we block but because we expect it to be fast
    reply.waitForFinished();
    if (reply.isError()) {
        auto err = reply.error();
        Logger::log(Logger::Error, "Error setting killing the daemon");
        setLastError(err);
    }
}

void
ManagerImpl::onWatcherDone() {
    auto senderObj = sender();
    senderObj->deleteLater();
}

}  // DownloadManager

}  // Ubuntu
