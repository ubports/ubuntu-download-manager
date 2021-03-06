/*
 * Copyright 2014-2015 Canonical Ltd.
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

#include "testing_file_download.h"

TestingFileDownload::TestingFileDownload(FileDownload* down,
                                         QObject* parent)
    : FileDownload(down->transferId(),
                   down->transferAppId(),
                   down->path(),
                   down->isConfined(),
                   down->rootPath(),
                   down->url(),
                   down->metadata(),
                   down->headers(), parent),
      _down(down) {
    // fwd all the diff signals
    connect(_down, &FileDownload::finished,
        this, &TestingFileDownload::finished);
    connect(_down, &Download::canceled,
        this, &TestingFileDownload::canceled);
    connect(_down, &Download::error,
        this, &TestingFileDownload::error);
    connect(_down, &Download::paused,
        this, &TestingFileDownload::paused);
    connect(_down, &Download::processing,
        this, &TestingFileDownload::processing);
    connect(_down, &Download::resumed,
        this, &TestingFileDownload::resumed);
    connect(_down, &Download::started,
        this, &TestingFileDownload::started);
    connect(_down, &Download::stateChanged,
        this, &TestingFileDownload::stateChanged);
    connect(_down, static_cast<void(Download::*)
        (qulonglong, qulonglong)>(&Download::progress),
	this, static_cast<void(Download::*)
	    (qulonglong, qulonglong)>(&Download::progress));
}

TestingFileDownload::~TestingFileDownload() {
    delete _down;
}

void
TestingFileDownload::returnDBusErrors(bool errors) {
    _returnErrors = errors;
}

void
TestingFileDownload::returnHttpError(HttpErrorStruct error) {
    _returnAuthError = false;
    _returnHttpError = true;
    _returnNetworkError = false;
     _returnProcessError = false;
    _httpErr = error;
}

void
TestingFileDownload::returnNetworkError(NetworkErrorStruct error) {
    _returnAuthError = false;
    _returnHttpError = false;
    _returnNetworkError = true;
     _returnProcessError = false;
    _networkErr = error;
}

void
TestingFileDownload::returnProcessError(ProcessErrorStruct error) {
    _returnAuthError = false;
    _returnHttpError = false;
    _returnNetworkError = false;
    _returnProcessError = true;
    _processErr = error;
}

void
TestingFileDownload::returnAuthError(AuthErrorStruct error) {
    _returnAuthError = true;
    _returnHttpError = false;
    _returnNetworkError = false;
    _returnProcessError = false;
    _returnHashError = false;
    _authErr = error;
}

void
TestingFileDownload::returnHashError(HashErrorStruct error) {
    _returnAuthError = false;
    _returnHttpError = false;
    _returnNetworkError = false;
    _returnProcessError = false;
    _returnHashError = true;
    _hashErr = error;
}

qulonglong
TestingFileDownload::progress() {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "progress");
    }
    return _down->progress();
}

qulonglong
TestingFileDownload::totalSize() {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "totalSize");
    }
    return _down->totalSize();
}

void
TestingFileDownload::setThrottle(qulonglong speed) {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "setThrottle");
    }
    _down->setThrottle(speed);
}

qulonglong
TestingFileDownload::throttle() {
    return _down->throttle();
}

void
TestingFileDownload::setHeaders(StringMap headers) {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "setThrottle");
    }
    _down->setHeaders(headers);
}

StringMap
TestingFileDownload::headers() {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "setThrottle");
    }
    return _down->headers();
}

void
TestingFileDownload::allowGSMDownload(bool allowed) {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "allowGSMDownload");
    }
    _down->allowGSMDownload(allowed);
}

bool
TestingFileDownload::isGSMDownloadAllowed() {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "isGSMDownloadAllowed");
    }
    return _down->isGSMDownloadAllowed();
}


QVariantMap
TestingFileDownload::metadata() const {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "metadata");
    }
    return _down->metadata();
}

void
TestingFileDownload::setMetadata(QVariantMap metadata) const {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "metadata");
    }
    return _down->setMetadata(metadata);
}

Ubuntu::DownloadManager::Daemon::Download::State
TestingFileDownload::state() const {
    return _down->state();
}

void
TestingFileDownload::cancel() {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "cancel");
    }
    _down->cancel();
}

void
TestingFileDownload::pause() {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "pause");
    }
    _down->pause();
}

void
TestingFileDownload::resume() {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "resume");
    }
    _down->resume();
}

void
TestingFileDownload::start() {
    qDebug() << "Starting download";
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "start");
    }

    _down->start();

    if (_returnAuthError) {
        emit authError(_authErr);
        emitError("Forced http error");
    }

    if (_returnHttpError) {
        emit httpError(_httpErr);
        emitError("Forced http error");
    }

    if (_returnNetworkError) {
        emit networkError(_networkErr);
        emitError("Forced network error");
    }

    if (_returnProcessError) {
        emit processError(_processErr);
        emitError("Forced processerror");
    }

    if (_returnHashError) {
        emit hashError(_hashErr);
        emitError("Forced hash error");
    }

}

void
TestingFileDownload::cancelTransfer() {
    _down->cancelTransfer();
}

void
TestingFileDownload::pauseTransfer() {
    _down->pauseTransfer();
}

void
TestingFileDownload::resumeTransfer() {
    _down->resumeTransfer();
}

void
TestingFileDownload::startTransfer() {
     qDebug() << "Start testing down";
    _down->startTransfer();
}
