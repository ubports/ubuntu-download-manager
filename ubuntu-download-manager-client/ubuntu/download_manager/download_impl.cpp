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

#include "download_impl.h"

namespace Ubuntu {

namespace DownloadManager {

DownloadImpl::DownloadImpl(const QDBusConnection& conn,
                           const QString& servicePath,
                           const QDBusObjectPath& objectPath,
                           QObject* parent)
    : Download(parent),
      _id(objectPath.path()),
      _conn(conn),
      _servicePath(servicePath) {
    _dbusInterface = new DownloadInterface(servicePath,
        _id, conn);

    // fwd all the signals but the error one
    connect(_dbusInterface, &DownloadInterface::canceled,
        this, &Download::canceled);
    connect(_dbusInterface, &DownloadInterface::finished,
        this, &Download::finished);
    connect(_dbusInterface, &DownloadInterface::paused,
        this, &Download::paused);
    connect(_dbusInterface, &DownloadInterface::processing,
        this, &Download::processing);
    connect(_dbusInterface, static_cast<void(DownloadInterface::*)
        (qulonglong, qulonglong)>(&DownloadInterface::progress),
        this, static_cast<void(Download::*)
            (qulonglong, qulonglong)>(&Download::progress));
    connect(_dbusInterface, &DownloadInterface::resumed,
        this, &Download::resumed);
    connect(_dbusInterface, &DownloadInterface::started,
        this, &Download::started);

    // connect to the different type of errors that will later be converted to
    // the error type to be used by the client.
    connect(_dbusInterface, &DownloadInterface::httpError,
        this, &DownloadImpl::onHttpError);
    connect(_dbusInterface, &DownloadInterface::networkError,
        this, &DownloadImpl::onNetworkError);
    connect(_dbusInterface, &DownloadInterface::processError,
        this, &DownloadImpl::onProcessError);
    connect(_dbusInterface, &DownloadInterface::authError,
        this, &DownloadImpl::onAuthError);
}

DownloadImpl::DownloadImpl(const QDBusConnection& conn, Error* err, QObject* parent)
    : Download(parent),
      _isError(true),
      _lastError(err),
      _conn(conn) {
}

DownloadImpl::~DownloadImpl() {
    delete _lastError;
    delete _dbusInterface;
}

void
DownloadImpl::setLastError(Error* err) {
    if (_lastError != nullptr) {
        delete _lastError;
    }
    _lastError = err;
    _isError = true;
    emit Download::error(err);
}

void
DownloadImpl::setLastError(const QDBusError& err) {
    setLastError(new DBusError(err, this));
}

void
DownloadImpl::start() {
    QDBusPendingCall call =
        _dbusInterface->start();
    auto watcher = new DownloadPendingCallWatcher(_conn, _servicePath,
        call, this);
    Q_UNUSED(watcher);
}

void
DownloadImpl::pause() {
    QDBusPendingCall call =
        _dbusInterface->pause();
    auto watcher = new DownloadPendingCallWatcher(_conn, _servicePath,
        call, this);
    Q_UNUSED(watcher);
}

void
DownloadImpl::resume() {
    QDBusPendingCall call =
        _dbusInterface->resume();
    auto watcher = new DownloadPendingCallWatcher(_conn, _servicePath,
        call, this);
    Q_UNUSED(watcher);
}

void
DownloadImpl::cancel() {
    QDBusPendingCall call =
        _dbusInterface->cancel();
    auto watcher = new DownloadPendingCallWatcher(_conn, _servicePath,
        call, this);
    Q_UNUSED(watcher);
}

void
DownloadImpl::allowMobileDownload(bool allowed) {
    QDBusPendingReply<> reply =
        _dbusInterface->allowGSMDownload(allowed);
    // block, the call should be fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        setLastError(reply.error());
    }
}

bool
DownloadImpl::isMobileDownloadAllowed() {
    QDBusPendingReply<bool> reply =
        _dbusInterface->isGSMDownloadAllowed();
    // block, the call should be fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        setLastError(reply.error());
        return false;
    } else {
        auto result = reply.value();
        return result;
    }
}

void
DownloadImpl::setThrottle(qulonglong speed) {
    QDBusPendingReply<> reply =
        _dbusInterface->setThrottle(speed);
    // block, the call should be fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        setLastError(reply.error());
    }
}

qulonglong
DownloadImpl::throttle() {
    QDBusPendingReply<qulonglong> reply =
        _dbusInterface->throttle();
    // block, the call is fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        setLastError(reply.error());
        return 0;
    } else {
        auto result = reply.value();
        return result;
    }
}

QString
DownloadImpl::id() const {
    return _id;
}

QVariantMap
DownloadImpl::metadata() {
    QDBusPendingReply<QVariantMap> reply =
        _dbusInterface->metadata();
    // block the call is fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        QVariantMap emptyResult;
        setLastError(reply.error());
        return emptyResult;
    } else {
        auto result = reply.value();
        return result;
    }
}

qulonglong
DownloadImpl::progress() {
    QDBusPendingReply<qulonglong> reply =
        _dbusInterface->progress();
    // block call should be fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        setLastError(reply.error());
        return 0;
    } else {
        auto result = reply.value();
        return result;
    }
}

qulonglong
DownloadImpl::totalSize() {
    QDBusPendingReply<qulonglong> reply =
        _dbusInterface->totalSize();
    // block call should b fast enough
    reply.waitForFinished();
    if (reply.isError()) {
        setLastError(reply.error());
        return 0;
    } else {
        auto result = reply.value();
        return result;
    }
}

bool
DownloadImpl::isError() const {
    return _isError;
}

Error*
DownloadImpl::error() const {
    return _lastError;
}

void
DownloadImpl::onHttpError(HttpErrorStruct errStruct) {
    auto err = new HttpError(errStruct, this);
    setLastError(err);
}

void
DownloadImpl::onNetworkError(NetworkErrorStruct errStruct) {
    auto err = new NetworkError(errStruct, this);
    setLastError(err);
}

void
DownloadImpl::onProcessError(ProcessErrorStruct errStruct) {
    auto err = new ProcessError(errStruct, this);
    setLastError(err);
}

void
DownloadImpl::onAuthError(AuthErrorStruct errStruct) {
    auto err = new AuthError(errStruct, this);
    setLastError(err);
}

}  // DownloadManager

}  // Ubuntu
