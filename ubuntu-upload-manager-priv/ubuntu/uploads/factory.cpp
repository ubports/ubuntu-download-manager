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
#include <ubuntu/transfers/system/logger.h>
#include "upload_adaptor.h"
#include "mms_file_upload.h"
#include "factory.h"

namespace {
    const QString OBJECT_PATH_KEY = "objectpath";
}

namespace Ubuntu {

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

void
Factory::getUploadPath(const QString& dbusOwner,
                       const QVariantMap& metadata,
                       QString& id,
                       QString& dbusPath,
                       bool& isConfined) {
    TRACE << dbusOwner;
    if (metadata.contains(OBJECT_PATH_KEY)) {
        // create a uuid using the string value form the metadata
        id = metadata[OBJECT_PATH_KEY].toString();
        if (id.isEmpty()) {
            LOG(ERROR) << "Id sent by client is ''";
            id = _apparmor->getSecurePath(dbusOwner, dbusPath, isConfined);
        } else {
            LOG(INFO) << "Using the id from the client:" << id;
            _apparmor->getSecurePath(dbusOwner, id, dbusPath, isConfined);
        }
    } else {
        LOG(INFO) << "Factory assigns the Download Uuid.";
        id = _apparmor->getSecurePath(dbusOwner, dbusPath, isConfined);
    }
}

FileUpload*
Factory::createUpload(const QString& dbusOwner,
                      const QUrl& url,
                      const QString& filePath,
                      const QVariantMap& metadata,
                      const QMap<QString, QString>& headers) {
    QString id;
    QString dbusPath;
    bool isConfined = false;
    getUploadPath(dbusOwner, metadata, id, dbusPath, isConfined);
    auto upload = new FileUpload(id, dbusPath, isConfined, url,
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
                         int port,
                         const QString& username,
                         const QString& password) {
    QNetworkProxy proxy(QNetworkProxy::HttpProxy, hostname,
        port, username, password);
    QString id;
    QString dbusPath;
    bool isConfined = false;
    QVariantMap metadata;
    QMap<QString, QString> headers;
    getUploadPath(dbusOwner, metadata, id, dbusPath, isConfined);
    auto upload = new MmsFileUpload(id, dbusPath, isConfined,
        url, filePath, metadata, headers, proxy);
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

