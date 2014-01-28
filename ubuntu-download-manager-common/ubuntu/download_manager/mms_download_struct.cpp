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

#include "mms_download_struct.h"

namespace Ubuntu {

namespace DownloadManager {

MmsDownloadStruct::MmsDownloadStruct()
    : _url(""),
      _hostName(""),
      _port(0),
      _user(""),
      _password("") {
}

MmsDownloadStruct::MmsDownloadStruct(const QString& url,
                                     const QString& hostName,
                                     int port,
                                     const QString& user,
                                     const QString& password)
    : _url(url),
      _hostName(hostName),
      _port(port),
      _user(user),
      _password(password) {
}

MmsDownloadStruct::MmsDownloadStruct(const MmsDownloadStruct& other)
    : _url(other._url),
      _hostName(other._hostName),
      _port(other._port),
      _user(other._user),
      _password(other._password) {
}

MmsDownloadStruct&
MmsDownloadStruct::operator=(const MmsDownloadStruct& other) {
    _url = other._url;
    _hostName = other._hostName;
    _port = other._port;
    _user = other._user;
    _password = other._password;
    return *this;
}

QDBusArgument &operator<<(QDBusArgument &argument,
                          const MmsDownloadStruct& download) {
    argument.beginStructure();
    argument << download._url;
    argument << download._hostName;
    argument << download._port;
    argument << download._user;
    argument << download._password;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument,
                                MmsDownloadStruct& download) {
    argument.beginStructure();
    argument >> download._url;
    argument >> download._hostName;
    argument >> download._port;
    argument >> download._user;
    argument >> download._password;
    argument.endStructure();

    return argument;
}

QString
MmsDownloadStruct::getUrl() {
    return _url;
}

QString
MmsDownloadStruct::getHostName() {
    return _hostName;
}

int
MmsDownloadStruct::getPort() {
    return _port;
}

QString
MmsDownloadStruct::getUser() {
    return _user;
}

QString
MmsDownloadStruct::getPassword() {
    return _password;
}

}  // DownloadManager

}  // Ubuntu
