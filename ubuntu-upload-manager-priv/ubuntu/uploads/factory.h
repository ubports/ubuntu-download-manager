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

#ifndef UPLOADER_LIB_UPLOAD_FACTORY_H
#define UPLOADER_LIB_UPLOAD_FACTORY_H

namespace Ubuntu {

namespace UploadManager {

namespace Daemon {

class Factory : public QObject {
    Q_OBJECT

 public:
    explicit Factory(QObject* parent = 0);
    Factory(AppArmor* apparmor,
            QObject *parent = 0);
    virtual ~Factory();

    virtual FileUpload* createUpload(const QString& dbusOwner,
                                       const QUrl& url,
                                       const QVariantMap& metadata,
                                       const QMap<QString, QString>& headers);

    virtual FileUpload* createMmsUpload(const QString& dbusOwner,
                                       const QUrl& url,
                                       const QVariantMap& metadata,
                                       const QMap<QString, QString>& headers);

    // mainly for testing purposes
    virtual QList<QSslCertificate> acceptedCertificates();
    virtual void setAcceptedCertificates(const QList<QSslCertificate>& certs);

 private:
    AppArmor* _apparmor;
};

}  // Daemon

}  // UploadManager

}  // Ubuntu

#endif
