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

#include <ubuntu/transfers/system/apn_request_factory.h>
#include "mms_file_download.h"

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

MmsFileDownload::MmsFileDownload(const QString& id,
                    const QString& appId,
                    const QString& path,
                    bool isConfined,
                    const QString& rootPath,
                    const QUrl& url,
                    const QVariantMap& metadata,
                    const QMap<QString, QString>& headers,
                    const QNetworkProxy& proxy,
                    QObject* parent)
    : FileDownload(id, appId, path, isConfined, rootPath, url,
                   metadata, headers, parent){
    _requestFactory = new ApnRequestFactory(proxy);
    setAddToQueue(false);
    // mms downloads should by default not be shown in the indicator.
    _metadata[Ubuntu::Transfers::Metadata::SHOW_IN_INDICATOR_KEY] = false;
}

MmsFileDownload::~MmsFileDownload() {
    _requestFactory->deleteLater();
}

}  // Daemon

}  // DownloadManager

}  // Ubuntu
