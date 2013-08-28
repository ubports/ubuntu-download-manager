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
#include "./system_network_info.h"
#include "./single_download.h"

class DownloadFactoryPrivate;
class DownloadFactory : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(DownloadFactory)

 public:
    DownloadFactory(SystemNetworkInfo* networkInfo,
                    RequestFactory* nam,
                    ProcessFactory* processFactory,
                    QObject *parent = 0);

    virtual SingleDownload* createDownload(const QUuid& id,
                                         const QString& path,
                                         const QUrl& url,
                                         const QVariantMap& metadata,
                                         const QMap<QString, QString>& headers);

    virtual SingleDownload* createDownload(const QUuid& id,
                                         const QString& path,
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
