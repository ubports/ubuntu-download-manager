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

#include <QNetworkProxy>
#include <QPair>
#include "ubuntu/system/logger.h"
#include "download_adaptor.h"
#include "group_download.h"
#include "group_download_adaptor.h"
#include "file_download.h"
#include "mms_file_download.h"
#include "factory.h"

namespace {

    const QString OBJECT_PATH_KEY = "objectpath";

}

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

Factory::Factory(QObject* parent)
    : QObject(parent) {
    _apparmor = new AppArmor(this);
}

Factory::Factory(AppArmor* apparmor,
                 QObject* parent)
    : QObject(parent),
        _apparmor(apparmor) {
}

Factory::~Factory() {
    delete _apparmor;
}

void
Factory::getDownloadPath(const QString& dbusOwner,
                         const QVariantMap& metadata,
                         QString& id,
                         QString& dbusPath,
                         QString& rootPath,
                         bool& isConfined) {
    TRACE << dbusOwner;
    if (metadata.contains(OBJECT_PATH_KEY)) {
        // create a uuid using the string value form the metadata
        id = metadata[OBJECT_PATH_KEY].toString();
        if (id.isEmpty()) {
            LOG(ERROR) << "Id sent by client is ''";
            id = _apparmor->getSecurePath(dbusOwner, dbusPath, rootPath,
                isConfined);
        } else {
            LOG(INFO) << "Using the id from the client:" << id;
            _apparmor->getSecurePath(dbusOwner, id, dbusPath, rootPath,
                isConfined);
        }
    } else {
        LOG(INFO) << "Factory assigns the Download Uuid.";
        id = _apparmor->getSecurePath(dbusOwner, dbusPath, rootPath,
            isConfined);
    }
}

Download*
Factory::createDownload(const QString& dbusOwner,
                        const QUrl& url,
                        const QVariantMap& metadata,
                        const QMap<QString, QString>& headers) {
    QString id;
    QString dbusPath;
    QString rootPath;
    bool isConfined = false;
    getDownloadPath(dbusOwner, metadata, id, dbusPath, rootPath,
        isConfined);
    Download* down = new FileDownload(id, dbusPath, isConfined, rootPath,
        url, metadata, headers);
    DownloadAdaptor* adaptor = new DownloadAdaptor(down);
    down->setAdaptor(adaptor);
    return down;
}

Download*
Factory::createDownload(const QString& dbusOwner,
                        const QUrl& url,
                        const QString& hash,
                        const QString& algo,
                        const QVariantMap& metadata,
                        const QMap<QString, QString>& headers) {
    QString id;
    QString dbusPath;
    QString rootPath;
    bool isConfined = false;
    getDownloadPath(dbusOwner, metadata, id, dbusPath, rootPath,
        isConfined);
    Download* down = new FileDownload(id, dbusPath, isConfined,
        rootPath, url, hash, algo, metadata, headers);
    DownloadAdaptor* adaptor = new DownloadAdaptor(down);
    down->setAdaptor(adaptor);
    return down;
}

Download*
Factory::createDownload(const QString& dbusOwner,
                        StructList downloads,
                        const QString& algo,
                        bool allowed3G,
                        const QVariantMap& metadata,
                        StringMap headers) {
    QString id;
    QString dbusPath;
    QString rootPath;
    bool isConfined = false;
    getDownloadPath(dbusOwner, metadata, id, dbusPath, rootPath,
        isConfined);
    Download* down = new GroupDownload(id, dbusPath, isConfined, rootPath,
        downloads, algo, allowed3G, metadata, headers, this);
    GroupDownloadAdaptor* adaptor = new GroupDownloadAdaptor(down);
    down->setAdaptor(adaptor);
    return down;
}

Download*
Factory::createMmsDownload(const QString& dbusOwner,
                           const QUrl& url,
                           const QString& hostname,
                           int port,
                           const QString& username,
                           const QString& password) {
    QNetworkProxy proxy(QNetworkProxy::HttpProxy, hostname,
        port, username, password);
    QString id;
    QString dbusPath;
    QString rootPath;
    bool isConfined = false;
    QVariantMap metadata;
    QMap<QString, QString> headers;
    getDownloadPath(dbusOwner, metadata, id, dbusPath, rootPath,
        isConfined);
    Download* down = new MmsFileDownload(id, dbusPath, isConfined,
        rootPath, url, metadata, headers, proxy);
    DownloadAdaptor* adaptor = new DownloadAdaptor(down);
    down->setAdaptor(adaptor);
    return down;
}

Download*
Factory::createDownloadForGroup(bool isConfined,
                                const QString& rootPath,
                                const QUrl& url,
                                const QVariantMap& metadata,
                                const QMap<QString, QString>& headers) {
    QString id;
    QString dbusPath;
    _apparmor->getDBusPath(id, dbusPath);
    Download* down = new FileDownload(id, dbusPath, isConfined, rootPath,
        url, metadata, headers);
    DownloadAdaptor* adaptor = new DownloadAdaptor(down);
    down->setAdaptor(adaptor);
    return down;
}

Download*
Factory::createDownloadForGroup(bool isConfined,
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
        rootPath, url, hash, algo, metadata, headers);
    DownloadAdaptor* adaptor = new DownloadAdaptor(down);
    down->setAdaptor(adaptor);
    return down;
}

QList<QSslCertificate>
Factory::acceptedCertificates() {
    return RequestFactory::instance()->acceptedCertificates();
}

void
Factory::setAcceptedCertificates(const QList<QSslCertificate>& certs) {
    RequestFactory::instance()->setAcceptedCertificates(certs);
}

}  // Daemon

}  // DownloadManager

}  // Ubuntu
