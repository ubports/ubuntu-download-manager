/*
 * Copyright 2015 Canonical Ltd.
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

#include "hash_error_struct.h"


namespace Ubuntu {

namespace Transfers {

namespace Errors {

HashErrorStruct::HashErrorStruct() {
}

HashErrorStruct::HashErrorStruct(const QString& method, const QString& expected, const QString& found)
    : _method(method),
      _expected(expected),
      _found(found) {

}

HashErrorStruct::HashErrorStruct(const HashErrorStruct& other)
    : _method(other._method),
      _expected(other._expected),
      _found(other._found) {
}

HashErrorStruct&
HashErrorStruct::operator=(const HashErrorStruct& other) {
    _method = other._method;
    _expected = other._expected;
    _found = other._found;

    return *this;
}

QDBusArgument&
operator<<(QDBusArgument &argument, const HashErrorStruct& error) {
    argument.beginStructure();
    argument << error._method;
    argument << error._expected;
    argument << error._found;
    argument.endStructure();

    return argument;
}

const QDBusArgument&
operator>>(const QDBusArgument &argument, HashErrorStruct& error) {
    argument.beginStructure();
    argument >> error._method;
    argument >> error._expected;
    argument >> error._found;
    argument.endStructure();

    return argument;
}

QString
HashErrorStruct::getMethod() const {
    return _method;
}

QString
HashErrorStruct::getExpected() const {
    return _expected;
}

QString
HashErrorStruct::getChecksum() const {
    return _found;
}

}

}

}
