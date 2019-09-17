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

#pragma once

class QDBusArgument;

namespace Ubuntu {

namespace Transfers {

namespace Errors {

/*!
    \class AuthErrorStruct 
    \brief The AuthErrorStruct represents the dbus structure that is used
           to communicate authentication errors that happened in the download
           manager to the different clients.
    \since 0.3
*/
class AuthErrorStruct {
    Q_PROPERTY(AuthErrorStruct::Type type READ getType)
    Q_PROPERTY(QString phrase READ getPhrase)
 public:

    /*!
        Enumerator that represents the error type.
    */
    enum Type {
        Server, /*! The server needed authentication and it was not provided.*/
        Proxy /*! The proxy needed authentication and it was not provided. */
    };

    /*!
        Creates a new structure with the default values.
    */
    AuthErrorStruct();

    /*!
        Creates a new structure with the given error types and message.
    */
    AuthErrorStruct(AuthErrorStruct::Type type, const QString& phrase);

    /*!
        Creates a new structure with the given error types and message.
    */
    AuthErrorStruct(int type, const QString& phrase);

    /*!
        Copy constructor.
    */
    AuthErrorStruct(const AuthErrorStruct& other);

    /*!
        Assign operator.
    */
    AuthErrorStruct& operator=(const AuthErrorStruct& other);

    /*!
        \internal
    */
    friend QDBusArgument &operator<<(QDBusArgument &argument,
        const AuthErrorStruct& error);

    /*!
        \internal
    */
    friend const QDBusArgument &operator>>(const QDBusArgument &argument,
        AuthErrorStruct& error);

    // properties getters

    /*!
        \fn AuthErrorStruct::Type getType() const

        Returns the type of the error.
    */
    AuthErrorStruct::Type getType() const;

    /*!
        \fn QString getPhrase() const

        Returns the message of the error.
    */
    QString getPhrase() const;

 private:

    /*!
        \internal
    */
    AuthErrorStruct::Type _type;

    /*!
        \internal
    */
    QString _phrase = QString();
};

}  // Errors

}  // Transfers

}  // Ubuntu
