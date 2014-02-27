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
#include "download.h"

FakeDownload::FakeDownload(const QString& id,
                           const QString& path,
                           bool isConfined,
                           const QString& rootPath,
                           const QUrl& url,
                           const QVariantMap& metadata,
                           const QMap<QString, QString> &headers,
                           QObject* parent)
        : FileDownload(id, path, isConfined, rootPath, url, metadata,
                headers, parent),
        _canTransfer(true) {
}

FakeDownload::FakeDownload(const QString& id,
                           const QString& path,
                           bool isConfined,
                           const QString& rootPath,
                           const QUrl& url,
                           const QString& hash,
                           const QString& algo,
                           const QVariantMap& metadata,
                           const QMap<QString, QString>& headers,
                           QObject* parent)
        : FileDownload(id, path, isConfined, rootPath, url, hash, algo,
                metadata, headers, parent),
        _canTransfer(true) {
}

bool
FakeDownload::canTransfer() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("canTransfer");
        _called.append(methodData);
    }
    return _canTransfer;
}

void
FakeDownload::setCanTransfer(bool canTransfer) {
    _canTransfer = canTransfer;
}

void
FakeDownload::setThrottle(qulonglong speed) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new UintWrapper(speed, this));

        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("setThrottle", params);
        _called.append(methodData);
    }
    FileDownload::setThrottle(speed);
}

qulonglong
FakeDownload::throttle() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("throttle");
        _called.append(methodData);
    }
    return FileDownload::throttle();
}

void
FakeDownload::cancelTransfer() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("cancelTransfer");
        _called.append(methodData);
    }
}

void
FakeDownload::pauseTransfer() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("pauseTransfer");
        _called.append(methodData);
    }
}

void
FakeDownload::resumeTransfer() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("resumeTransfer");
        _called.append(methodData);
    }
}

void
FakeDownload::startTransfer() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("startTransfer");
        _called.append(methodData);
    }
}

void
FakeDownload::emitFinished(const QString& path) {
    qDebug() << __PRETTY_FUNCTION__;
    setState(Download::FINISH);
    emit finished(path);
}

void
FakeDownload::emitError(const QString& errorMsg) {
    qDebug() << __PRETTY_FUNCTION__;
    setState(Download::ERROR);
    emit error(errorMsg);
}

void
FakeDownload::emitAuthError(AuthErrorStruct error) {
    emit authError(error);
}

void
FakeDownload::emitHttpError(HttpErrorStruct error) {
    emit httpError(error);
}

void
FakeDownload::emitNetworkError(NetworkErrorStruct error) {
    emit networkError(error);
}

void
FakeDownload::emitProcessError(ProcessErrorStruct error) {
    emit processError(error);
}

