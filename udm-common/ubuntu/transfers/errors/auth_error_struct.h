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

#ifndef DOWNLOADER_LIB_AUTH_ERROR_STRUCT_H
#define DOWNLOADER_LIB_AUTH_ERROR_STRUCT_H

class QDBusArgument;

namespace Ubuntu {

namespace Transfers {

namespace Errors {

class AuthErrorStruct {
    Q_PROPERTY(AuthErrorStruct::Type type READ getType)
    Q_PROPERTY(QString phrase READ getPhrase)
 public:
    enum Type {
        Server,
        Proxy
    };

    AuthErrorStruct();
    AuthErrorStruct(AuthErrorStruct::Type type, QString phrase);
    AuthErrorStruct(int type, QString phrase);
    AuthErrorStruct(const AuthErrorStruct& other);
    AuthErrorStruct& operator=(const AuthErrorStruct& other);

    friend QDBusArgument &operator<<(QDBusArgument &argument,
        const AuthErrorStruct& error);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument,
        AuthErrorStruct& error);

    // properties getters
    AuthErrorStruct::Type getType();
    QString getPhrase();

 private:
    AuthErrorStruct::Type _type;
    QString _phrase;
};

}  // Errors

}  // Transfers

}  // Ubuntu
#endif // AUTH_ERROR_STRUCT_H
