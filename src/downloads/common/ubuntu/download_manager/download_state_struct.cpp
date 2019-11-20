/*
 * Copyright 2013-2014 Canonical Ltd.
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
#include "download_state_struct.h"

namespace Ubuntu {

namespace DownloadManager {

DownloadStateStruct::DownloadStateStruct()
    : _state(-1),
      _url(QString()),
      _filePath(QString()),
      _hash(QString()),
      _metadata(QVariantMap()) {

}

DownloadStateStruct::DownloadStateStruct(int state, const QString& url, const QString& hash)
    : _state(state),
      _url(url),
      _filePath(QString()),
      _hash(hash),
      _metadata(QVariantMap()) {

}

DownloadStateStruct::DownloadStateStruct(int state, const QString& url, const QString& filePath,
                                         const QString& hash)
        : _state(state),
          _url(url),
          _filePath(filePath),
          _hash(hash),
          _metadata(QVariantMap()) {

}

DownloadStateStruct::DownloadStateStruct(int state, const QString& url, const QString& filePath,
                                         const QString& hash, const QVariantMap& metadata)
        : _state(state),
          _url(url),
          _filePath(filePath),
          _hash(hash),
          _metadata(metadata) {

}

DownloadStateStruct::DownloadStateStruct(const DownloadStateStruct& other)
        : _state(other._state),
          _url(other._url),
          _filePath(other._filePath),
          _hash(other._hash),
          _metadata(other._metadata) {
}

DownloadStateStruct& DownloadStateStruct::operator=(const DownloadStateStruct& other) {
    _state = other._state;
    _url = other._url;
    _filePath= other._filePath;
    _hash = other._hash;
    _metadata = other._metadata;

    return *this;
}

QDBusArgument &operator<<(QDBusArgument &argument,
                          const DownloadStateStruct& download) {
    argument.beginStructure();
    argument << download._state;
    argument << download._url;
    argument << download._filePath;
    argument << download._hash;
    argument << download._metadata;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument,
                                DownloadStateStruct& download) {
    argument.beginStructure();
    argument >> download._state;
    argument >> download._url;
    argument >> download._filePath;
    argument >> download._hash;
    argument >> download._metadata;
    argument.endStructure();

    return argument;
}

int
DownloadStateStruct::getState() const {
    return _state;
}

QString
DownloadStateStruct::getUrl() const {
    return _url;
}

QString
DownloadStateStruct::getFilePath() const {
    return _filePath;
}

QString
DownloadStateStruct::getHash() const {
    return _hash;
}

QVariantMap
DownloadStateStruct::getMetadata() const {
    return _metadata;
}

bool
DownloadStateStruct::isValid() {
    return _url != QString();
}

}  // DownloadManager

}  // Ubuntu

