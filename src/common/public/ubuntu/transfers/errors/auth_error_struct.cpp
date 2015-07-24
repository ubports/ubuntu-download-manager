/*
 * Copyright 2014-2015 Canonical Ltd.
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
#include "auth_error_struct.h"

namespace Ubuntu {

namespace Transfers {

namespace Errors {

AuthErrorStruct::AuthErrorStruct()
    : _type(),
      _phrase("") {
}

AuthErrorStruct::AuthErrorStruct(AuthErrorStruct::Type type, const QString& phrase)
    : _type(type),
      _phrase(phrase) {
}

AuthErrorStruct::AuthErrorStruct(int type, const QString& phrase)
    : _type(static_cast<AuthErrorStruct::Type>(type)),
      _phrase(phrase) {
}

AuthErrorStruct::AuthErrorStruct(const AuthErrorStruct& other)
    : _type(other._type),
      _phrase(other._phrase){
}

AuthErrorStruct&
AuthErrorStruct::operator=(const AuthErrorStruct& other) {
    _type = other._type;
    _phrase = other._phrase;

    return *this;
}

QDBusArgument&
operator<<(QDBusArgument &argument,
           const AuthErrorStruct& error) {
    argument.beginStructure();
    argument << static_cast<int>(error._type);
    argument << error._phrase;
    argument.endStructure();

    return argument;
}

const QDBusArgument&
operator>>(const QDBusArgument &argument,
           AuthErrorStruct& error) {
    int tempType;
    argument.beginStructure();
    argument >> tempType;
    argument >> error._phrase;
    argument.endStructure();

    error._type = static_cast<AuthErrorStruct::Type>(tempType);

    return argument;
}


AuthErrorStruct::Type
AuthErrorStruct::getType() const {
    return _type;
}

QString
AuthErrorStruct::getPhrase() const {
    return _phrase;
}

}  // Errors

}  // Transfers

}  // Ubuntu
