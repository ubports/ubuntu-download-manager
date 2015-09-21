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
      _url(QString::null),
      _filePath(QString::null),
      _hash(QString::null) {

}

DownloadStateStruct::DownloadStateStruct(int state, const QString& url, const QString& hash)
    : _state(state),
      _url(url),
      _filePath(QString::null),
      _hash(hash) {

}

DownloadStateStruct::DownloadStateStruct(int state, const QString& url, const QString& filePath,
                                         const QString& hash)
        : _state(state),
          _url(url),
          _filePath(filePath),
          _hash(hash) {

}

DownloadStateStruct::DownloadStateStruct(const DownloadStateStruct& other)
        : _state(other._state),
          _url(other._url),
          _filePath(other._filePath),
          _hash(other._hash) {
}

DownloadStateStruct& DownloadStateStruct::operator=(const DownloadStateStruct& other) {
    _state = other._state;
    _url = other._url;
    _filePath= other._filePath;
    _hash = other._hash;

    return *this;
}

QDBusArgument &operator<<(QDBusArgument &argument,
                          const DownloadStateStruct& download) {
    argument.beginStructure();
    argument << download._state;
    argument << download._url;
    argument << download._filePath;
    argument << download._hash;
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

bool
DownloadStateStruct::isValid() {
    return _url != QString::null;
}

}  // DownloadManager

}  // Ubuntu

