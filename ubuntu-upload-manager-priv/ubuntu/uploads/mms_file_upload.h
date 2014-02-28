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

#ifndef MMS_FILE_UPLOAD_H
#define MMS_FILE_UPLOAD_H

#include <QNetworkProxy>
#include <QObject>
#include "file_upload.h"

namespace Ubuntu {

namespace UploadManager {

namespace Daemon {

class MmsFileUpload : public FileUpload {
    Q_OBJECT

 public:
    MmsFileUpload(const QString& id,
                  const QString& path,
                  bool isConfined,
                  const QUrl& url,
                  const QString& filePath,
                  const QVariantMap& metadata,
                  const QMap<QString, QString>& headers,
                  const QNetworkProxy& proxy,
                  QObject* parent = 0);
    ~MmsFileUpload();
};

}  // Daemon

}  // UploadManager

}  // Ubunut

#endif
