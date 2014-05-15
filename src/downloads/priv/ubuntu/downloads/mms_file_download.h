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

#ifndef MMS_FILE_DOWNLOAD_H
#define MMS_FILE_DOWNLOAD_H

#include <QNetworkProxy>
#include <QObject>
#include "file_download.h"

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

class MmsFileDownload : public FileDownload {
    Q_OBJECT

 public:
    MmsFileDownload(const QString& id,
                    const QString& appId,
                    const QString& path,
                    bool isConfined,
                    const QString& rootPath,
                    const QUrl& url,
                    const QVariantMap& metadata,
                    const QMap<QString, QString>& headers,
                    const QNetworkProxy& proxy,
                    QObject* parent = 0);
    ~MmsFileDownload();
};

}  // Daemon

}  // DownloadManager

}  // Ubunut

#endif // MMS_FILE_DOWNLOAD_H
