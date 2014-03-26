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

#include <QDBusArgument>
#include "upload_struct.h"

namespace Ubuntu {

namespace UploadManager {

UploadStruct::UploadStruct()
    : _url(""),
      _filePath(""),
      _metadata(),
      _headers() {
}

UploadStruct::UploadStruct(const QString& url,
                           const QString& filePath)
    : _url(url),
      _filePath(filePath),
      _metadata(),
      _headers() {
}

UploadStruct::UploadStruct(const QString& url,
                           const QString& filePath,
                           const QVariantMap& metadata,
                           const QMap<QString, QString>& headers)
    : _url(url),
      _filePath(filePath),
      _metadata(metadata),
      _headers(headers) {
}

UploadStruct::UploadStruct(const UploadStruct& other)
    : _url(other._url),
      _filePath(other._filePath),
      _metadata(other._metadata),
      _headers(other._headers) {
}

UploadStruct& UploadStruct::operator=(const UploadStruct& other) {
    _url = other._url;
    _filePath = other._filePath;
    _metadata = other._metadata;
    _headers = other._headers;

    return *this;
}

QDBusArgument&
operator<<(QDBusArgument &argument, const UploadStruct& upload) {
    argument.beginStructure();
    argument << upload._url;
    argument << upload._filePath;
    argument << upload._metadata;
    argument << upload._headers;
    argument.endStructure();

    return argument;
}

const QDBusArgument&
operator>>(const QDBusArgument &argument, UploadStruct& upload) {
    argument.beginStructure();
    argument >> upload._url;
    argument >> upload._filePath;
    argument >> upload._metadata;
    argument >> upload._headers;
    argument.endStructure();

    return argument;
}

QString
UploadStruct::getUrl() {
    return _url;
}

QString
UploadStruct::getFilePath() {
    return _filePath;
}

QVariantMap
UploadStruct::getMetadata() {
    return _metadata;
}

QMap<QString, QString>
UploadStruct::getHeaders() {
    return _headers;
}

}  // UploadManager

}  // Ubuntu
