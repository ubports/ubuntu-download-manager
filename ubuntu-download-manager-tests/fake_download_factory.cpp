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

#include "./fake_download.h"
#include "./fake_download_factory.h"

FakeDownloadFactory::FakeDownloadFactory(SystemNetworkInfo* networkInfo,
                                         RequestFactory* nam,
                                         ProcessFactory* processFactory,
                                         QObject *parent)
    : DownloadFactory(networkInfo, nam, processFactory, parent),
      Fake(),
      _networkInfo(networkInfo),
      _nam(nam),
      _processFactory(processFactory) {
}

SingleDownload*
FakeDownloadFactory::createDownload(const QUuid& id,
                                    const QString& path,
                                    const QUrl& url,
                                    const QVariantMap& metadata,
                                    const QMap<QString, QString>& headers) {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("createDownload");
        _called.append(methodData);
    }
    SingleDownload* down = new FakeDownload(id, path, url, metadata, headers,
        _networkInfo, _nam, _processFactory);
    _downloads.append(down);
    return down;
}

SingleDownload*
FakeDownloadFactory::createDownload(const QUuid& id,
                                    const QString& path,
                                    const QUrl& url,
                                    const QString& hash,
                                    QCryptographicHash::Algorithm algo,
                                    const QVariantMap& metadata,
                                    const QMap<QString, QString>& headers) {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("createDownload");
        _called.append(methodData);
    }
    SingleDownload* down = new FakeDownload(id, path, url, hash, algo,
        metadata, headers, _networkInfo, _nam, _processFactory);
    _downloads.append(down);
    return down;
}

QList<SingleDownload*>
FakeDownloadFactory::downloads() {
    return _downloads;
}
