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

#include <QCryptographicHash>
#include <QObject>
#include <QSharedPointer>
#include "./apparmor.h"
#include "./metatypes.h"
#include "./system_network_info.h"
#include "./download.h"
#include "./uuid_factory.h"

class DownloadFactoryPrivate;
class DownloadFactory : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(DownloadFactory)

 public:
    DownloadFactory(QSharedPointer<AppArmor> apparmor,
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
                                     QCryptographicHash::Algorithm algo,
                                     const QVariantMap& metadata,
                                     const QMap<QString, QString>& headers);

    virtual Download* createDownload(const QString& dbusOwner,
                                     StructList downloads,
                                     QCryptographicHash::Algorithm algo,
                                     bool allowed3G,
                                     const QVariantMap& metadata,
                                     StringMap headers);

    // create downloads from a group download

    virtual Download* createDownload(bool isConfined,
                                     const QString& rootPath,
                                     const QUrl& url,
                                     const QVariantMap& metadata,
                                     const QMap<QString, QString>& headers);

    virtual Download* createDownload(bool isConfined,
                                     const QString& rootPath,
                                     const QUrl& url,
                                     const QString& hash,
                                     QCryptographicHash::Algorithm algo,
                                     const QVariantMap& metadata,
                                     const QMap<QString, QString>& headers);

 private:
    // use pimpl so that we can mantains ABI compatibility
    DownloadFactoryPrivate* d_ptr;
};

#endif  // DOWNLOADER_LIB_DOWNLOAD_FACTORY_H
