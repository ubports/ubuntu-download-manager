/*
 * Copyright 2014 Canonical Ltd.
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
#include "upload_adaptor.h"
#include "mms_file_upload.h"
#include "factory.h"

namespace Ubuntu {

using namespace Transfers;

namespace UploadManager {

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

FileUpload*
Factory::createUpload(const QString& dbusOwner,
                      const QUrl& url,
                      const QString& filePath,
                      const QVariantMap& metadata,
                      const QMap<QString, QString>& headers) {
    QScopedPointer<SecurityDetails> details(
        getSecurityDetails(dbusOwner, metadata));
    auto upload = new FileUpload(details->id, details->appId,
            details->dbusPath, details->isConfined, url,
	    filePath, metadata, headers);
    auto adaptor = new UploadAdaptor(upload);
    upload->setAdaptor(adaptor);
    return upload;
}

FileUpload*
Factory::createMmsUpload(const QString& dbusOwner,
                         const QUrl& url,
                         const QString& filePath,
                         const QString& hostname,
                         int port) {
    QNetworkProxy proxy(QNetworkProxy::HttpProxy, hostname,
        port);
    DLOG(INFO) << "Proxy created for " << hostname << ":" << port;
    QVariantMap metadata;
    QMap<QString, QString> headers;
    QScopedPointer<SecurityDetails> details(
        getSecurityDetails(dbusOwner, metadata));
    auto upload = new MmsFileUpload(details->id, "", details->dbusPath,
        details->isConfined, url, filePath, metadata, headers, proxy);
    auto adaptor = new UploadAdaptor(upload);
    upload->setAdaptor(adaptor);
    return upload;
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

