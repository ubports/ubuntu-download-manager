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

#include "./download_factory.h"

/*
 * PRIVATE IMPLEMENTATION
 */


class DownloadFactoryPrivate {
    Q_DECLARE_PUBLIC(DownloadFactory)

 public:
    DownloadFactoryPrivate(SystemNetworkInfo* networkInfo,
                           RequestFactory* nam,
                           ProcessFactory* processFactory,
                           DownloadFactory* parent)
        : _networkInfo(networkInfo),
          _nam(nam),
          _processFactory(processFactory),
          q_ptr(parent) {
    }

    SingleDownload* createDownload(const QUuid& id,
                                   const QString& path,
                                   const QUrl& url,
                                   const QVariantMap& metadata,
                                   const QMap<QString, QString>& headers) {
        return new SingleDownload(id, path, url, metadata, headers,
            _networkInfo, _nam, _processFactory);
    }

    SingleDownload* createDownload(const QUuid& id,
                                   const QString& path,
                                   const QUrl& url,
                                   const QString& hash,
                                   QCryptographicHash::Algorithm algo,
                                   const QVariantMap& metadata,
                                   const QMap<QString, QString>& headers) {
        return new SingleDownload(id, path, url, hash, algo, metadata, headers,
            _networkInfo, _nam, _processFactory);
    }

 private:
    SystemNetworkInfo* _networkInfo;
    RequestFactory* _nam;
    ProcessFactory* _processFactory;
    DownloadFactory* q_ptr;
};

/*
 * PUBLIC IMPLEMENTATION
 */

DownloadFactory::DownloadFactory(SystemNetworkInfo* networkInfo,
                                 RequestFactory* nam,
                                 ProcessFactory* processFactory,
                                 QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadFactoryPrivate(networkInfo, nam, processFactory,
      this)) {
}


SingleDownload*
DownloadFactory::createDownload(const QUuid& id,
                                const QString& path,
                                const QUrl& url,
                                const QVariantMap& metadata,
                                const QMap<QString, QString>& headers) {
    Q_D(DownloadFactory);
    return d->createDownload(id, path, url, metadata, headers);
}

SingleDownload*
DownloadFactory::createDownload(const QUuid& id,
                                const QString& path,
                                const QUrl& url,
                                const QString& hash,
                                QCryptographicHash::Algorithm algo,
                                const QVariantMap& metadata,
                                const QMap<QString, QString>& headers) {
    Q_D(DownloadFactory);
    return d->createDownload(id, path, url, hash, algo, metadata, headers);
}
