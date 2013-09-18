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

#define OBJECT_PATH_KEY "objectpath"

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

    void getDownloadPath(const QString& dbusOwner,
                         const QVariantMap& metadata,
                         QUuid& id,
                         QString& dbusPath,
                         QString& rootPath,
                         bool& isConfined) {
        qDebug() << __PRETTY_FUNCTION__ << dbusOwner << metadata;
        if (metadata.contains(OBJECT_PATH_KEY)) {
            // create a uuid using the string value form the metadata
            id = QUuid(metadata[OBJECT_PATH_KEY].toString());
            if (id.isNull()) {
                qCritical() << "Uuid sent by client is NULL";
                id = _apparmor->getSecurePath(dbusOwner, dbusPath, rootPath,
                    isConfined);
            } else {
                qDebug() << "Using the id from the client" << id;
                _apparmor->getSecurePath(dbusOwner, id, dbusPath, rootPath,
                    isConfined);
            }
        } else {
            qDebug() << "DownloadFactory assigns the Download Uuid.";
            id = _apparmor->getSecurePath(dbusOwner, dbusPath, rootPath,
                isConfined);
        }
    }

    Download* createDownload(const QString& dbusOwner,
                             const QUrl& url,
                             const QVariantMap& metadata,
                             const QMap<QString, QString>& headers) {
        QUuid id;
        QString dbusPath;
        QString rootPath;
        bool isConfined = false;
        getDownloadPath(dbusOwner, metadata, id, dbusPath, rootPath,
            isConfined);
        qDebug() << "Download secure data is " << id << dbusPath << rootPath;
        Download* down = new SingleDownload(id, dbusPath, isConfined, rootPath,
            url, metadata, headers, _networkInfo, _nam, _processFactory);
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
        QUuid id;
        QString dbusPath;
        QString rootPath;
        bool isConfined = false;
        getDownloadPath(dbusOwner, metadata, id, dbusPath, rootPath,
            isConfined);
        qDebug() << "Download secure data is " << id << dbusPath << rootPath;
        Download* down = new SingleDownload(id, dbusPath, isConfined,
            rootPath, url, hash, algo, metadata, headers, _networkInfo, _nam,
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
        QUuid id;
        QString dbusPath;
        QString rootPath;
        bool isConfined = false;
        getDownloadPath(dbusOwner, metadata, id, dbusPath, rootPath,
            isConfined);
        qDebug() << "Download secure data is " << id << dbusPath << rootPath;
        Download* down = new GroupDownload(id, dbusPath, isConfined, rootPath,
            downloads, algo, allowed3G, metadata, headers, _networkInfo,
            _self);
        GroupDownloadAdaptor* adaptor = new GroupDownloadAdaptor(down);
        down->setAdaptor(adaptor);
        return down;
    }

    Download* createDownloadForGroup(bool isConfined,
                                     const QString& rootPath,
                                     const QUrl& url,
                                     const QVariantMap& metadata,
                                     const QMap<QString, QString>& headers) {
        QUuid id;
        QString dbusPath;
        _apparmor->getDBusPath(id, dbusPath);
        Download* down = new SingleDownload(id, dbusPath, isConfined, rootPath,
            url, metadata, headers, _networkInfo, _nam, _processFactory);
        DownloadAdaptor* adaptor = new DownloadAdaptor(down);
        down->setAdaptor(adaptor);
        return down;
    }

    Download* createDownloadForGroup(bool isConfined,
                                     const QString& rootPath,
                                     const QUrl& url,
                                     const QString& hash,
                                     QCryptographicHash::Algorithm algo,
                                     const QVariantMap& metadata,
                                     const QMap<QString, QString>& headers) {
        QUuid id;
        QString dbusPath;
        _apparmor->getDBusPath(id, dbusPath);
        Download* down = new SingleDownload(id, dbusPath, isConfined,
            rootPath, url, hash, algo, metadata, headers, _networkInfo, _nam,
            _processFactory);
        DownloadAdaptor* adaptor = new DownloadAdaptor(down);
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

Download*
DownloadFactory::createDownloadForGroup(bool isConfined,
                                        const QString& rootPath,
                                        const QUrl& url,
                                        const QVariantMap& metadata,
                                        const QMap<QString, QString>& headers) {
    Q_D(DownloadFactory);
    return d->createDownloadForGroup(isConfined, rootPath, url, metadata,
        headers);
}

Download*
DownloadFactory::createDownloadForGroup(bool isConfined,
                                        const QString& rootPath,
                                        const QUrl& url,
                                        const QString& hash,
                                        QCryptographicHash::Algorithm algo,
                                        const QVariantMap& metadata,
                                        const QMap<QString, QString>& headers) {
    Q_D(DownloadFactory);
    return d->createDownloadForGroup(isConfined, rootPath, url, hash, algo,
        metadata, headers);
}
