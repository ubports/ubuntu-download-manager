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

#include "network_error_struct.h"


namespace Ubuntu {

namespace DownloadManager {

NetworkErrorStruct::NetworkErrorStruct()
    :  _code(0),
       _phrase("No error condition.") {
}

NetworkErrorStruct::NetworkErrorStruct(int code, QString phrase)
    : _code(code),
      _phrase(phrase) {
}

NetworkErrorStruct::NetworkErrorStruct(const NetworkErrorStruct& other)
    : _code(other._code),
      _phrase(other._phrase) {
}

NetworkErrorStruct&
NetworkErrorStruct::operator=(const NetworkErrorStruct& other) {
    _code = other._code;
    _phrase = other._phrase;

    return *this;
}

QDBusArgument &operator<<(QDBusArgument &argument,
                          const NetworkErrorStruct& error) {
    argument.beginStructure();
    argument << error._code;
    argument << error._phrase;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument,
                                NetworkErrorStruct& error) {
    argument.beginStructure();
    argument >> error._code;
    argument >> error._phrase;
    argument.endStructure();

    return argument;
}

int
NetworkErrorStruct::getCode() {
    return _code;
}

QString
NetworkErrorStruct::getPhrase() {
    return _phrase;
}

}  // DownloadManager

}  // Ubuntu
