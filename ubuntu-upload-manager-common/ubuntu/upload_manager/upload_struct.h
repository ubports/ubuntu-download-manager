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

#ifndef UPLOADER_LIB_UPLOAD_STRUCT_H
#define UPLOADER_LIB_UPLOAD_STRUCT_H

#include <QMap>
#include <QString>
#include <QVariantMap>
#include "common.h"

class QDBusArgument;
namespace Ubuntu {

namespace UploadManager {

typedef QMap<QString, QString> StringList;
class UPLOAD_MANAGER_EXPORT UploadStruct {
    Q_PROPERTY(QString url READ getUrl)
    Q_PROPERTY(QString filePath READ getFilePath)
    Q_PROPERTY(QVariantMap metadata READ getMetadata)
    Q_PROPERTY(StringList headers READ getHeaders)

 public:
    UploadStruct();
    UploadStruct(const QString& url,
                 const QString& filePath);
    UploadStruct(const QString& url,
                 const QString& filePath,
                 const QVariantMap& metadata,
                 const QMap<QString, QString>& headers);
    UploadStruct(const UploadStruct& other);
    UploadStruct& operator=(const UploadStruct& other);
    ~UploadStruct();

    friend QDBusArgument &operator<<(QDBusArgument &argument,
        const UploadStruct& upload);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument,
        UploadStruct& upload);

    // properties getters
    QString getUrl();
    QString getFilePath();
    QVariantMap getMetadata();
    QMap<QString, QString> getHeaders();

 private:
    QString _url;
    QString _filePath;
    QVariantMap _metadata;
    QMap<QString, QString> _headers;
};

}  // UploadManager

}  // Ubuntu

#endif  // UPLOADER_LIB_UPLOAD_STRUCT_H
