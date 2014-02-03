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
#include <QDBusConnection>
#include <QDBusObjectPath>
#include <ubuntu/download_manager/metatypes.h>
#include "download_pendingcall_watcher.h"
#include "download_interface.h"
#include "error.h"
#include "download.h"

namespace Ubuntu {

namespace DownloadManager {

/*
 * PRIVATE IMPLEMENTATION
 */

class DownloadPrivate {
    Q_DECLARE_PUBLIC(Download)

 public:
    DownloadPrivate(const QDBusConnection& conn,
                    const QString& servicePath,
                    const QDBusObjectPath& objectPath,
                    Download* parent)
        : _id(objectPath.path()),
          _conn(conn),
          _servicePath(servicePath),
          q_ptr(parent) {
        Q_Q(Download);
        _dbusInterface = new DownloadInterface(servicePath,
            _id, conn);

        // fwd all the signals but the error one
        q->connect(_dbusInterface, &DownloadInterface::canceled,
            q, &Download::canceled);
        q->connect(_dbusInterface, &DownloadInterface::finished,
            q, &Download::finished);
        q->connect(_dbusInterface, &DownloadInterface::paused,
            q, &Download::paused);
        q->connect(_dbusInterface, &DownloadInterface::processing,
            q, &Download::processing);
        q->connect(_dbusInterface, static_cast<void(DownloadInterface::*)
            (qulonglong, qulonglong)>(&DownloadInterface::progress),
            q, static_cast<void(Download::*)
                (qulonglong, qulonglong)>(&Download::progress));
        q->connect(_dbusInterface, &DownloadInterface::resumed,
            q, &Download::resumed);
        q->connect(_dbusInterface, &DownloadInterface::started,
            q, &Download::started);

        // connect to the different type of errors that will later be converted to
        // the error type to be used by the client. With a pimpl implementation we
        // cannot use the new stype connections :(
        q->connect(_dbusInterface, SIGNAL(httpError(HttpErrorStruct error)),
            q, SLOT(onHttpError(HttpErrorStruct)));
        q->connect(_dbusInterface, SIGNAL(networkError(NetworkErrorStruct error)),
            q, SLOT(onNetworkError(NetworkErrorStruct)));
        q->connect(_dbusInterface, SIGNAL(processError(ProcessErrorStruct error)),
            q, SLOT(onProcessError(ProcessErrorStruct)));
    }

    DownloadPrivate(const QDBusConnection& conn, Error* err, Download* parent)
        : _isError(true),
          _lastError(err),
          _conn(conn),
          q_ptr(parent) {
    }

    ~DownloadPrivate() {
        delete _lastError;
        delete _dbusInterface;
    }

    void setLastError(Error* err) {
        Q_Q(Download);
        if (_lastError != nullptr) {
            delete _lastError;
        }
        _lastError = err;
        _isError = true;
        emit q->error(err);
    }

    void setLastError(const QDBusError& err) {
        Q_Q(Download);
        setLastError(new DBusError(err, q));
    }

    void start() {
        Q_Q(Download);
        QDBusPendingCall call =
            _dbusInterface->start();
        auto watcher = new DownloadPendingCallWatcher(_conn, _servicePath,
            call, q);
        Q_UNUSED(watcher);
    }

    void pause() {
        Q_Q(Download);
        QDBusPendingCall call =
            _dbusInterface->pause();
        auto watcher = new DownloadPendingCallWatcher(_conn, _servicePath,
            call, q);
        Q_UNUSED(watcher);
    }

    void resume() {
        Q_Q(Download);
        QDBusPendingCall call =
            _dbusInterface->resume();
        auto watcher = new DownloadPendingCallWatcher(_conn, _servicePath,
            call, q);
        Q_UNUSED(watcher);
    }

    void cancel() {
        Q_Q(Download);
        QDBusPendingCall call =
            _dbusInterface->cancel();
        auto watcher = new DownloadPendingCallWatcher(_conn, _servicePath,
            call, q);
        Q_UNUSED(watcher);
    }

    void allowMobileDownload(bool allowed) {
        QDBusPendingReply<> reply =
            _dbusInterface->allowGSMDownload(allowed);
        // block, the call should be fast enough
        reply.waitForFinished();
        if (reply.isError()) {
            setLastError(reply.error());
        }
    }

    bool isMobileDownloadAllowed() {
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

    void setThrottle(qulonglong speed) {
        QDBusPendingReply<> reply =
            _dbusInterface->setThrottle(speed);
        // block, the call should be fast enough
        reply.waitForFinished();
        if (reply.isError()) {
            setLastError(reply.error());
        }
    }

    qulonglong throttle() {
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

    QString id() {
        return _id;
    }

    QVariantMap metadata() {
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

    qulonglong progress() {
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

    qulonglong totalSize() {
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

    bool isError() {
        return _isError;
    }

    Error* error() {
        return _lastError;
    }

    void onHttpError(HttpErrorStruct errStruct) {
        Q_Q(Download);
        auto err = new HttpError(errStruct, q);
        setLastError(err);
    }

    void onNetworkError(NetworkErrorStruct errStruct) {
        Q_Q(Download);
        auto err = new NetworkError(errStruct, q);
        setLastError(err);
    }

    void onProcessError(ProcessErrorStruct errStruct) {
        Q_Q(Download);
        auto err = new ProcessError(errStruct, q);
        setLastError(err);
    }

 private:
    QString _id;
    bool _isError = false;
    Error* _lastError = nullptr;
    DownloadInterface* _dbusInterface = nullptr;
    QDBusConnection _conn;
    QString _servicePath;
    Download* q_ptr;
};

Download::Download(const QDBusConnection& conn, Error* err, QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadPrivate(conn, err, this)) {
}

Download::Download(const QDBusConnection& conn,
                   const QString& servicePath,
                   const QDBusObjectPath& objectPath,
                   QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadPrivate(conn, servicePath, objectPath, this)) {
}

Download::~Download() {
    delete d_ptr;
}

void
Download::start(){
    Q_D(Download);
    d->start();
}

void
Download::pause(){
    Q_D(Download);
    d->pause();
}

void
Download::resume(){
    Q_D(Download);
    d->resume();
}

void
Download::cancel(){
    Q_D(Download);
    d->cancel();
}

void
Download::allowMobileDownload(bool allowed){
    Q_D(Download);
    d->allowMobileDownload(allowed);
}

bool
Download::isMobileDownloadAllowed(){
    Q_D(Download);
    return d->isMobileDownloadAllowed();
}

void
Download::setThrottle(qulonglong speed){
    Q_D(Download);
    d->setThrottle(speed);
}

qulonglong
Download::throttle(){
    Q_D(Download);
    return d->throttle();
}

QString
Download::id() {
    Q_D(Download);
    return d->id();
}

QVariantMap
Download::metadata(){
    Q_D(Download);
    return d->metadata();
}

qulonglong
Download::progress(){
    Q_D(Download);
    return d->progress();
}

qulonglong
Download::totalSize(){
    Q_D(Download);
    return d->totalSize();
}

bool
Download::isError() {
    Q_D(Download);
    return d->isError();
}

Error*
Download::error() {
    Q_D(Download);
    return d->error();
}

}  // DownloadManager

}  // Ubuntu

#include "moc_download.cpp"
