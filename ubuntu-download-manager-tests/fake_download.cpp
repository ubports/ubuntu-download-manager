/*
 * Copyright 2013 2013 Canonical Ltd.
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
#include "fake_download.h"

FakeDownload::FakeDownload(const QString& id,
                           const QString& path,
                           bool isConfined,
                           const QString& rootPath,
                           const QUrl& url,
                           const QVariantMap& metadata,
                           const QMap<QString, QString> &headers,
                           QSharedPointer<SystemNetworkInfo> networkInfo,
                           QSharedPointer<RequestFactory> nam,
                           QSharedPointer<ProcessFactory> processFactory,
                           QObject* parent)
        : SingleDownload(id, path, isConfined, rootPath, url, metadata,
                headers, networkInfo, nam, processFactory, parent),
        _canDownload(true) {
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
                           QSharedPointer<SystemNetworkInfo> networkInfo,
                           QSharedPointer<RequestFactory> nam,
                           QSharedPointer<ProcessFactory> processFactory,
                           QObject* parent)
        : SingleDownload(id, path, isConfined, rootPath, url, hash, algo,
                metadata, headers, networkInfo, nam, processFactory, parent),
        _canDownload(true) {
}

bool
FakeDownload::canDownload() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("canDownload");
        _called.append(methodData);
    }
    return _canDownload;
}

void
FakeDownload::setCanDownload(bool canDownload) {
    _canDownload = canDownload;
}

void
FakeDownload::setThrottle(qulonglong speed) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new UintWrapper(speed));

        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("setThrottle", params);
        _called.append(methodData);
    }
    SingleDownload::setThrottle(speed);
}

qulonglong
FakeDownload::throttle() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("throttle");
        _called.append(methodData);
    }
    return SingleDownload::throttle();
}

void
FakeDownload::cancelDownload() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("cancelDownload");
        _called.append(methodData);
    }
}

void
FakeDownload::pauseDownload() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("pauseDownload");
        _called.append(methodData);
    }
}

void
FakeDownload::resumeDownload() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("resumeDownload");
        _called.append(methodData);
    }
}

void
FakeDownload::startDownload() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("startDownload");
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
