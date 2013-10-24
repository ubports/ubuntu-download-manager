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

#ifndef DOWNLOADER_LIB_DOWNLOAD_FACTORY_H
#define DOWNLOADER_LIB_DOWNLOAD_FACTORY_H

#include <QObject>
#include <QSharedPointer>
#include "metatypes.h"
#include "downloads/download.h"
#include "system/apparmor.h"
#include "system/system_network_info.h"
#include "system/uuid_factory.h"

class FactoryPrivate;
class Factory : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(Factory)

 public:
    Factory(QSharedPointer<AppArmor> apparmor,
            QSharedPointer<SystemNetworkInfo> networkInfo,
            QSharedPointer<RequestFactory> nam,
            QSharedPointer<ProcessFactory> processFactory,
            QObject *parent = 0);

    // create downloads comming from a dbus call

    virtual Download* createDownload(const QString& dbusOwner,
                                     const QUrl& url,
                                     const QVariantMap& metadata,
                                     const QMap<QString, QString>& headers);

    virtual Download* createDownload(const QString& dbusOwner,
                                     const QUrl& url,
                                     const QString& hash,
                                     const QString& algo,
                                     const QVariantMap& metadata,
                                     const QMap<QString, QString>& headers);

    virtual Download* createDownload(const QString& dbusOwner,
                                     StructList downloads,
                                     const QString& algo,
                                     bool allowed3G,
                                     const QVariantMap& metadata,
                                     StringMap headers);

    // create downloads from a group download

    virtual Download* createDownloadForGroup(bool isConfined,
                                         const QString& rootPath,
                                         const QUrl& url,
                                         const QVariantMap& metadata,
                                         const QMap<QString, QString>& headers);

    virtual Download* createDownloadForGroup(bool isConfined,
                                         const QString& rootPath,
                                         const QUrl& url,
                                         const QString& hash,
                                         const QString& algo,
                                         const QVariantMap& metadata,
                                         const QMap<QString, QString>& headers);

    // mainly for testing purposes

    virtual QList<QSslCertificate> acceptedCertificates();
    virtual void setAcceptedCertificates(const QList<QSslCertificate>& certs);

 private:
    // use pimpl so that we can mantains ABI compatibility
    FactoryPrivate* d_ptr;
};

#endif  // DOWNLOADER_LIB_DOWNLOAD_FACTORY_H
