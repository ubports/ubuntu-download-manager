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

#include "fake_download.h"
#include "fake_download_factory.h"

FakeDownloadFactory::FakeDownloadFactory(
                                 QSharedPointer<AppArmor> apparmor,
                                 SystemNetworkInfo* networkInfo,
                                 QSharedPointer<RequestFactory> nam,
                                 QSharedPointer<ProcessFactory> processFactory,
                                 QObject *parent)
    : Factory(apparmor, networkInfo, nam, processFactory, parent),
      Fake(),
      _apparmor(apparmor),
      _networkInfo(networkInfo),
      _nam(nam),
      _processFactory(processFactory) {
}

Download*
FakeDownloadFactory::createDownload(const QString& downloadOwner,
                                    const QUrl& url,
                                    const QVariantMap& metadata,
                                    const QMap<QString, QString>& headers) {
    qDebug() << __PRETTY_FUNCTION__;
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("createDownload");
        _called.append(methodData);
    }
    QString dbusPath;
    QString rootPath;
    bool isConfined;
    QString id = _apparmor->getSecurePath(downloadOwner, dbusPath, rootPath,
        isConfined);
    Download* down = new FakeDownload(id, dbusPath, isConfined, rootPath, url,
        metadata, headers, _networkInfo, _nam, _processFactory);
    _downloads.append(down);
    qDebug() << "Downloads count" << _downloads.count();
    return down;
}

Download*
FakeDownloadFactory::createDownload(const QString& downloadOwner,
                                    const QUrl& url,
                                    const QString& hash,
                                    const QString& algo,
                                    const QVariantMap& metadata,
                                    const QMap<QString, QString>& headers) {
    qDebug() << __PRETTY_FUNCTION__;
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("createDownload");
        _called.append(methodData);
    }
    QString dbusPath;
    QString rootPath;
    bool isConfined;
    QString id = _apparmor->getSecurePath(downloadOwner, dbusPath, rootPath,
        isConfined);
    Download* down = new FakeDownload(id, dbusPath, isConfined, rootPath, url,
        hash, algo, metadata, headers, _networkInfo, _nam, _processFactory);
    _downloads.append(down);
    qDebug() << "Downloads count" << _downloads.count();
    return down;
}

Download*
FakeDownloadFactory::createDownload(const QString& downloadOwner,
                                    StructList downloads,
                                    const QString& algo,
                                    bool allowed3G,
                                    const QVariantMap& metadata,
                                    StringMap headers) {
    qDebug() << __PRETTY_FUNCTION__;
    Q_UNUSED(allowed3G);
    Q_UNUSED(downloads);
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("createDownload");
        _called.append(methodData);
    }

    QString dbusPath;
    QString rootPath;
    bool isConfined;
    QString id = _apparmor->getSecurePath(downloadOwner, dbusPath, rootPath,
        isConfined);
    Download* down = new FakeDownload(id, dbusPath, isConfined, rootPath,
        QUrl(), "", algo, metadata, headers, _networkInfo, _nam,
        _processFactory);
    _downloads.append(down);
    qDebug() << "Downloads count" << _downloads.count();
    return down;
}

Download*
FakeDownloadFactory::createDownloadForGroup(bool isConfined,
                                    const QString& rootPath,
                                    const QUrl& url,
                                    const QVariantMap& metadata,
                                    const QMap<QString, QString>& headers)  {
    qDebug() << __PRETTY_FUNCTION__;
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("createDownload");
        _called.append(methodData);
    }
    QString id;
    QString dbusPath;
    _apparmor->getDBusPath(id, dbusPath);
    Download* down = new FakeDownload(id, dbusPath, isConfined, rootPath,
        url, metadata, headers, _networkInfo, _nam, _processFactory);
    _downloads.append(down);
    qDebug() << "Downloads count" << _downloads.count();
    return down;
}

Download*
FakeDownloadFactory::createDownloadForGroup(bool isConfined,
                                    const QString& rootPath,
                                    const QUrl& url,
                                    const QString& hash,
                                    const QString& algo,
                                    const QVariantMap& metadata,
                                    const QMap<QString, QString>& headers) {
    QString id;
    QString dbusPath;
    _apparmor->getDBusPath(id, dbusPath);
    Download* down = new FileDownload(id, dbusPath, isConfined,
        rootPath, url, hash, algo, metadata, headers, _networkInfo, _nam,
        _processFactory);
    _downloads.append(down);
    qDebug() << "Downloads count" << _downloads.count();
    return down;
}

QList<Download*>
FakeDownloadFactory::downloads() {
    return _downloads;
}
