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

#include <QDBusConnection>
#include <QDBusObjectPath>
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
        : q_ptr(parent) {
        _dbusInterface = new DownloadInterface(servicePath,
            objectPath.path(), conn);
    }

    DownloadPrivate(Error* err, Download* parent)
        : _isError(true),
          _lastError(err),
          q_ptr(parent) {
    }

    ~DownloadPrivate() {
        delete _lastError;
        delete _dbusInterface;
    }

    void setLastError(const QDBusError& err) {
        Q_Q(Download);
        // delete the last if error if present to keep mem to a minimum
        if (_lastError != nullptr) {
            delete _lastError;
        }
        _lastError = new Error(err, q);
        _isError = true;
    }

    void start() {
    }

    void pause() {
    }

    void resume() {
    }

    void cancel() {
    }

    void allowMobileDownload(bool allowed) {
        Q_UNUSED(allowed);
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
        Q_UNUSED(speed);
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

 private:
    bool _isError = false;
    Error* _lastError = nullptr;
    DownloadInterface* _dbusInterface = nullptr;
    Download* q_ptr;

};

Download::Download(Error* err, QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadPrivate(err, this)) {
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
