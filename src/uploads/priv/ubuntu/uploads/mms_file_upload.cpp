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
#include "mms_file_upload.h"

namespace {
    const QString& MMS_CONTENT_TYPE = "application/vnd.wap.mms-message";
}

namespace Ubuntu {

namespace UploadManager {

namespace Daemon {

MmsFileUpload::MmsFileUpload(const QString& id,
                    const QString& appId,
                    const QString& path,
                    bool isConfined,
                    const QString& rootPath,
                    const QUrl& url,
                    const QString& filePath,
                    const QVariantMap& metadata,
                    const QMap<QString, QString>& headers,
                    const QNetworkProxy& proxy,
                    QObject* parent)
    : FileUpload(id, appId, path, isConfined, rootPath, url, filePath,
                   metadata, headers, parent){
    _requestFactory = new ApnRequestFactory(proxy);
    setAddToQueue(false);
}

MmsFileUpload::~MmsFileUpload() {
    _requestFactory->deleteLater();
}

QNetworkRequest
MmsFileUpload::setRequestHeaders(QNetworkRequest request) {
    auto r = FileUpload::setRequestHeaders(request);
    r.setHeader(QNetworkRequest::ContentTypeHeader,
        MMS_CONTENT_TYPE);
    return r;
}

}  // Daemon

}  // UploadManager

}  // Ubuntu

