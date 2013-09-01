/*
 * Copyright 2013 Canonical Ltd.
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

#include <QPair>
#include "./download_adaptor.h"
#include "./group_download.h"
#include "./group_download_adaptor.h"
#include "./single_download.h"
#include "./download_factory.h"

/*
 * PRIVATE IMPLEMENTATION
 */


class DownloadFactoryPrivate {
    Q_DECLARE_PUBLIC(DownloadFactory)

 public:
    explicit DownloadFactoryPrivate(DownloadFactory* parent)
        : q_ptr(parent) {
        _uuidFactory = new UuidFactory();
        _networkInfo = new SystemNetworkInfo();
        _nam = new RequestFactory();
        _processFactory = new ProcessFactory();
    }

    DownloadFactoryPrivate(UuidFactory* uuidFactory,
                           SystemNetworkInfo* networkInfo,
                           RequestFactory* nam,
                           ProcessFactory* processFactory,
                           DownloadFactory* parent)
        : _uuidFactory(uuidFactory),
          _networkInfo(networkInfo),
          _nam(nam),
          _processFactory(processFactory),
          q_ptr(parent) {
    }

    QPair<QUuid, QString> getDownloadId() {
        QUuid id = _uuidFactory->createUuid();
        QString uuidString = id.toString().replace(QRegExp("[-{}]"), "");
        QString path = DownloadFactoryPrivate::BASE_ACCOUNT_URL.arg(uuidString);
        return QPair<QUuid, QString>(id, path);
    }

    Download* createDownload(const QUrl& url,
                                   const QVariantMap& metadata,
                                   const QMap<QString, QString>& headers) {
        QPair<QUuid, QString> idData = getDownloadId();
        Download* down = new SingleDownload(idData.first, idData.second, url,
            metadata, headers, _networkInfo, _nam, _processFactory);
        DownloadAdaptor* adaptor = new DownloadAdaptor(down);
        down->setAdaptor(adaptor);
        return down;
    }

    Download* createDownload(const QUrl& url,
                             const QString& hash,
                             QCryptographicHash::Algorithm algo,
                             const QVariantMap& metadata,
                             const QMap<QString, QString>& headers) {
        QPair<QUuid, QString> idData = getDownloadId();
        Download* down = new SingleDownload(idData.first, idData.second, url,
            hash, algo, metadata, headers, _networkInfo, _nam,
            _processFactory);
        DownloadAdaptor* adaptor = new DownloadAdaptor(down);
        down->setAdaptor(adaptor);
        return down;
    }

    Download* createDownload(StructList downloads,
                             QCryptographicHash::Algorithm algo,
                             bool allowed3G,
                             const QVariantMap& metadata,
                             StringMap headers) {
        QPair<QUuid, QString> idData = getDownloadId();
        Download* down = new GroupDownload(idData.first, idData.second,
            downloads, algo, allowed3G, metadata, headers, _networkInfo,
            _nam, _processFactory);
        GroupDownloadAdaptor* adaptor = new GroupDownloadAdaptor(down);
        down->setAdaptor(adaptor);
        return down;
    }

 private:
    static QString BASE_ACCOUNT_URL;

    UuidFactory* _uuidFactory;
    SystemNetworkInfo* _networkInfo;
    RequestFactory* _nam;
    ProcessFactory* _processFactory;
    DownloadFactory* q_ptr;
};

QString DownloadFactoryPrivate::BASE_ACCOUNT_URL =
    "/com/canonical/applications/download/%1";

/*
 * PUBLIC IMPLEMENTATION
 */

DownloadFactory::DownloadFactory(QObject *parent)
    : QObject(parent),
      d_ptr(new DownloadFactoryPrivate(this)) {
}

DownloadFactory::DownloadFactory(UuidFactory* uuidFactory,
                                 SystemNetworkInfo* networkInfo,
                                 RequestFactory* nam,
                                 ProcessFactory* processFactory,
                                 QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadFactoryPrivate(uuidFactory, networkInfo, nam,
      processFactory, this)) {
}


Download*
DownloadFactory::createDownload(const QUrl& url,
                                const QVariantMap& metadata,
                                const QMap<QString, QString>& headers) {
    Q_D(DownloadFactory);
    return d->createDownload(url, metadata, headers);
}

Download*
DownloadFactory::createDownload(const QUrl& url,
                                const QString& hash,
                                QCryptographicHash::Algorithm algo,
                                const QVariantMap& metadata,
                                const QMap<QString, QString>& headers) {
    Q_D(DownloadFactory);
    return d->createDownload(url, hash, algo, metadata, headers);
}

Download*
DownloadFactory::createDownload(StructList downloads,
                                QCryptographicHash::Algorithm algo,
                                bool allowed3G,
                                const QVariantMap& metadata,
                                StringMap headers) {
    Q_D(DownloadFactory);
    return d->createDownload(downloads, algo, allowed3G, metadata,
        headers);
}
