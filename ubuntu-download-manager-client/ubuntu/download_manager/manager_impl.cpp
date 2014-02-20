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
#include "manager_impl.h"

namespace {
    const QString MANAGER_PATH = "/";
}

namespace Ubuntu {

namespace DownloadManager {

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
    qRegisterMetaType<ProcessError*>("ProcessError*");
    qRegisterMetaType<DownloadsList*>("DownloadsList*");
    qDBusRegisterMetaType<StringMap>();
    qDBusRegisterMetaType<DownloadStruct>();
    qDBusRegisterMetaType<GroupDownloadStruct>();
    qDBusRegisterMetaType<StructList>();
    qDBusRegisterMetaType<HttpErrorStruct>();
    qDBusRegisterMetaType<NetworkErrorStruct>();
    qDBusRegisterMetaType<ProcessErrorStruct>();
}

void
ManagerImpl::createDownload(DownloadStruct downStruct) {
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
    connect(watcher, &DownloadManagerPCW::callbackExecuted,
        this, &ManagerImpl::onWatcherDone);
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
    connect(watcher, &GroupManagerPCW::callbackExecuted,
        this, &ManagerImpl::onWatcherDone);
}

void
ManagerImpl::getAllDownloads() {
    DownloadsListCb cb = [](DownloadsList*){};
    getAllDownloads(cb, cb);
}

void
ManagerImpl::getAllDownloads(DownloadsListCb cb, DownloadsListCb errCb) {
    QDBusPendingCall call = _dbusInterface->getAllDownloads();
    auto watcher = new DownloadsListManagerPCW(
        _conn, _servicePath, call, cb, errCb, this);
    connect(watcher, &GroupManagerPCW::callbackExecuted,
        this, &ManagerImpl::onWatcherDone);
}

void
ManagerImpl::getAllDownloadsWithMetadata(const QString &name,
                                         const QString &value) {
    MetadataDownloadsListCb cb =
        [](const QString&, const QString&, DownloadsList*){};
    getAllDownloadsWithMetadata(name, value, cb, cb);
}

void
ManagerImpl::getAllDownloadsWithMetadata(const QString &name,
                                         const QString &value,
                                         MetadataDownloadsListCb cb,
                                         MetadataDownloadsListCb errCb) {
    QDBusPendingCall call = _dbusInterface->getAllDownloadsWithMetadata(
    name, value);
    auto watcher = new MetadataDownloadsListManagerPCW(
        _conn, _servicePath, call, name, value, cb, errCb, this);
    connect(watcher, &GroupManagerPCW::callbackExecuted,
        this, &ManagerImpl::onWatcherDone);
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

bool
ManagerImpl::isMobileDataDownload() {
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

qulonglong
ManagerImpl::defaultThrottle() {
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

void
ManagerImpl::setDefaultThrottle(qulonglong speed) {
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

void
ManagerImpl::exit() {
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

void
ManagerImpl::onWatcherDone() {
    auto senderObj = sender();
    senderObj->deleteLater();
}

}  // DownloadManager

}  // Ubuntu
