/*
 * Copyright 2013 2013 Canonical Ltd.
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

#include "downloads/download_struct.h"

namespace Ubuntu {

namespace DownloadManager {

DownloadStruct::DownloadStruct()
    : _url(""),
      _hash(""),
      _algorithm(""),
      _metadata(),
      _headers() {
}

DownloadStruct::DownloadStruct(const QString& url,
               const QVariantMap& metadata,
               const QMap<QString, QString>& headers)
    : _url(url),
      _hash(""),
      _algorithm(""),
      _metadata(metadata),
      _headers(headers) {
}

DownloadStruct::DownloadStruct(const QString& url,
               const QString& hash,
               const QString& algorithm,
               const QVariantMap& metadata,
               const QMap<QString, QString>& headers)
    : _url(url),
      _hash(hash),
      _algorithm(algorithm),
      _metadata(metadata),
      _headers(headers) {
}

DownloadStruct::DownloadStruct(const DownloadStruct& other)
    : _url(other._url),
      _hash(other._hash),
      _algorithm(other._algorithm),
      _metadata(other._metadata),
      _headers(other._headers) {
}

DownloadStruct& DownloadStruct::operator=(const DownloadStruct& other) {
    _url = other._url;
    _hash = other._hash;
    _algorithm = other._algorithm;
    _metadata = other._metadata;
    _headers = other._headers;

    return *this;
}

DownloadStruct::~DownloadStruct() {
}

QDBusArgument &operator<<(QDBusArgument &argument,
                          const DownloadStruct& download) {
    argument.beginStructure();
    argument << download._url;
    argument << download._hash;
    argument << download._algorithm;
    argument << download._metadata;
    argument << download._headers;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument,
                                DownloadStruct& download) {
    argument.beginStructure();
    argument >> download._url;
    argument >> download._hash;
    argument >> download._algorithm;
    argument >> download._metadata;
    argument >> download._headers;
    argument.endStructure();

    return argument;
}

QString
DownloadStruct::getUrl() {
    return _url;
}

QString
DownloadStruct::getHash() {
    return _hash;
}

QString
DownloadStruct::getAlgorithm() {
    return _algorithm;
}

QVariantMap
DownloadStruct::getMetadata() {
    return _metadata;
}

QMap<QString, QString>
DownloadStruct::getHeaders() {
    return _headers;
}

}  // DownloadManager

}  // Ubuntu
