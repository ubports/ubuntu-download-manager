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
    DownloadFactoryPrivate(QSharedPointer<AppArmor> apparmor,
                           QSharedPointer<SystemNetworkInfo> networkInfo,
                           QSharedPointer<RequestFactory> nam,
                           QSharedPointer<ProcessFactory> processFactory,
                           DownloadFactory* parent)
        : _apparmor(apparmor),
          _networkInfo(networkInfo),
          _nam(nam),
          _processFactory(processFactory),
          _self(parent),
          q_ptr(parent) {
    }

    Download* createDownload(const QString& dbusOwner,
                             const QUrl& url,
                             const QVariantMap& metadata,
                             const QMap<QString, QString>& headers) {
        QPair<QUuid, QString> idData = _apparmor->getSecurePath(dbusOwner);
        Download* down = new SingleDownload(idData.first, idData.second, url,
            metadata, headers, _networkInfo, _nam, _processFactory);
        DownloadAdaptor* adaptor = new DownloadAdaptor(down);
        down->setAdaptor(adaptor);
        return down;
    }

    Download* createDownload(const QString& dbusOwner,
                             const QUrl& url,
                             const QString& hash,
                             QCryptographicHash::Algorithm algo,
                             const QVariantMap& metadata,
                             const QMap<QString, QString>& headers) {
        QPair<QUuid, QString> idData = _apparmor->getSecurePath(dbusOwner);
        Download* down = new SingleDownload(idData.first, idData.second, url,
            hash, algo, metadata, headers, _networkInfo, _nam,
            _processFactory);
        DownloadAdaptor* adaptor = new DownloadAdaptor(down);
        down->setAdaptor(adaptor);
        return down;
    }

    Download* createDownload(const QString& dbusOwner,
                             StructList downloads,
                             QCryptographicHash::Algorithm algo,
                             bool allowed3G,
                             const QVariantMap& metadata,
                             StringMap headers) {
        QPair<QUuid, QString> idData = _apparmor->getSecurePath(dbusOwner);
        Download* down = new GroupDownload(idData.first, idData.second,
            downloads, algo, allowed3G, metadata, headers, _networkInfo,
            _self);
        GroupDownloadAdaptor* adaptor = new GroupDownloadAdaptor(down);
        down->setAdaptor(adaptor);
        return down;
    }

 private:
    QSharedPointer<AppArmor> _apparmor;
    QSharedPointer<SystemNetworkInfo> _networkInfo;
    QSharedPointer<RequestFactory> _nam;
    QSharedPointer<ProcessFactory> _processFactory;
    QSharedPointer<DownloadFactory> _self;
    DownloadFactory* q_ptr;
};

/*
 * PUBLIC IMPLEMENTATION
 */

DownloadFactory::DownloadFactory(QSharedPointer<AppArmor> apparmor,
                                 QSharedPointer<SystemNetworkInfo> networkInfo,
                                 QSharedPointer<RequestFactory> nam,
                                 QSharedPointer<ProcessFactory> processFactory,
                                 QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadFactoryPrivate(apparmor, networkInfo, nam,
        processFactory, this)) {
}


Download*
DownloadFactory::createDownload(const QString& dbusOwner,
                                const QUrl& url,
                                const QVariantMap& metadata,
                                const QMap<QString, QString>& headers) {
    Q_D(DownloadFactory);
    return d->createDownload(dbusOwner, url, metadata, headers);
}

Download*
DownloadFactory::createDownload(const QString& dbusOwner,
                                const QUrl& url,
                                const QString& hash,
                                QCryptographicHash::Algorithm algo,
                                const QVariantMap& metadata,
                                const QMap<QString, QString>& headers) {
    Q_D(DownloadFactory);
    return d->createDownload(dbusOwner, url, hash, algo, metadata, headers);
}

Download*
DownloadFactory::createDownload(const QString& dbusOwner,
                                StructList downloads,
                                QCryptographicHash::Algorithm algo,
                                bool allowed3G,
                                const QVariantMap& metadata,
                                StringMap headers) {
    Q_D(DownloadFactory);
    return d->createDownload(dbusOwner, downloads, algo, allowed3G, metadata,
        headers);
}
