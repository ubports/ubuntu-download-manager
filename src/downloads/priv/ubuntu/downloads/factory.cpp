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
#include <ubuntu/transfers/metadata.h>
#include <ubuntu/transfers/system/logger.h>
#include "download_adaptor.h"
#include "group_download.h"
#include "group_download_adaptor.h"
#include "file_download.h"
#include "mms_file_download.h"
#include "factory.h"

namespace {
    const QString DOWNLOAD_INTERFACE = "com.canonical.applications.Download";
    const QString GROUP_INTERFACE = "com.canonical.applications.GroupDownload";
    const QString PROPERTY_INTERFACE = "org.freedesktop.DBus.Properties";
}

namespace Ubuntu {

using namespace Transfers;

namespace DownloadManager {

namespace Daemon {

Factory::Factory(AppArmor* apparmor,
                 QObject* parent)
    : QObject(parent),
        _apparmor(apparmor) {
}

Factory::~Factory() {
    delete _apparmor;
}

SecurityDetails*
Factory::getSecurityDetails(const QString& dbusOwner,
                            const QVariantMap& metadata) {
    TRACE << dbusOwner;
    QString id;
    if (metadata.contains(Metadata::OBJECT_PATH_KEY)) {
        // create a uuid using the string value form the metadata
        id = metadata[Metadata::OBJECT_PATH_KEY].toString();
        if (id.isEmpty()) {
            LOG(ERROR) << "Id sent by client is ''";
            return _apparmor->getSecurityDetails(dbusOwner);
        } else {
            LOG(INFO) << "Using the id from the client:" << id;
            return _apparmor->getSecurityDetails(dbusOwner, id);
        }
    } else {
        LOG(INFO) << "Factory assigns the Download Uuid.";
        return _apparmor->getSecurityDetails(dbusOwner);
    }
}

Download*
Factory::createDownload(const QString& dbusOwner,
                        const QUrl& url,
                        const QVariantMap& metadata,
                        const QMap<QString, QString>& headers) {
    QScopedPointer<SecurityDetails> details(
        getSecurityDetails(dbusOwner, metadata));
    auto dbusPath = details->dbusPath.arg("download");
    auto down = new FileDownload(details->id, details->appId,
        dbusPath, details->isConfined, details->localPath, url, metadata, headers);
    auto downAdaptor = new DownloadAdaptor(down);
    down->setAdaptor(DOWNLOAD_INTERFACE, downAdaptor);
    return down;
}

Download*
Factory::createDownload(const QString& dbusOwner,
                        const QUrl& url,
                        const QString& hash,
                        const QString& algo,
                        const QVariantMap& metadata,
                        const QMap<QString, QString>& headers) {
    QScopedPointer<SecurityDetails> details(
        getSecurityDetails(dbusOwner, metadata));
    auto dbusPath = details->dbusPath.arg("download");
    auto down = new FileDownload(details->id, details->appId,
        dbusPath, details->isConfined, details->localPath,
        url, hash, algo, metadata, headers);
    auto downAdaptor = new DownloadAdaptor(down);
    down->setAdaptor(DOWNLOAD_INTERFACE, downAdaptor);
    return down;
}

Download*
Factory::createDownload(const QString& dbusOwner,
                        StructList downloads,
                        const QString& algo,
                        bool allowed3G,
                        const QVariantMap& metadata,
                        StringMap headers) {
    QScopedPointer<SecurityDetails> details(
        getSecurityDetails(dbusOwner, metadata));
    auto dbusPath = details->dbusPath.arg("download");
    auto down = new GroupDownload(details->id, details->appId,
        dbusPath, details->isConfined, details->localPath,
        downloads, algo, allowed3G, metadata, headers, this);
    auto downAdaptor = new GroupDownloadAdaptor(down);
    down->setAdaptor(GROUP_INTERFACE, downAdaptor);
    return down;
}

Download*
Factory::createMmsDownload(const QString& dbusOwner,
                           const QUrl& url,
                           const QString& hostname,
                           int port) {
    QNetworkProxy proxy(QNetworkProxy::HttpProxy, hostname,
        port);
    QVariantMap metadata;
    QMap<QString, QString> headers;
    QScopedPointer<SecurityDetails> details(
        getSecurityDetails(dbusOwner, metadata));
    auto dbusPath = details->dbusPath.arg("download");
    auto down = new MmsFileDownload(details->id, details->appId,
        dbusPath, details->isConfined, details->localPath,
        url, metadata, headers, proxy);
    auto downAdaptor = new DownloadAdaptor(down);
    down->setAdaptor(DOWNLOAD_INTERFACE, downAdaptor);
    return down;
}

Download*
Factory::createDownloadForGroup(bool isConfined,
                                const QString& rootPath,
                                const QUrl& url,
                                const QVariantMap& metadata,
                                const QMap<QString, QString>& headers) {
    auto idAndPath = _apparmor->getDBusPath();
    auto down = new FileDownload(idAndPath.first, "",
        idAndPath.second.arg("download"), isConfined, rootPath, url,
        metadata, headers);
    auto downAdaptor = new DownloadAdaptor(down);
    down->setAdaptor(DOWNLOAD_INTERFACE, downAdaptor);
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
    auto idAndPath = _apparmor->getDBusPath();
    auto down = new FileDownload(idAndPath.first, "",
        idAndPath.second.arg("download"), isConfined, rootPath, url, hash,
        algo, metadata, headers);
    auto downAdaptor = new DownloadAdaptor(down);
    down->setAdaptor(DOWNLOAD_INTERFACE, downAdaptor);
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
