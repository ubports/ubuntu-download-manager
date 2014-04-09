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

#ifndef DOWNLOADER_LIB_NETWORK_ERROR_STRUCT_H
#define DOWNLOADER_LIB_NETWORK_ERROR_STRUCT_H

#include <QString>

class QDBusArgument;

namespace Ubuntu {

namespace Transfers {

namespace Errors {

/*!
    \class NetworkErrorStruct 
    \brief The NetworkErrorStruct represents the dbus structure that is used
           to communicate network errors that happened in the download
           manager to the different clients.
    \since 0.3
*/
class NetworkErrorStruct {
    Q_PROPERTY(int code READ getCode)
    Q_PROPERTY(QString phrase READ getPhrase)

 public:

    /*!
        Creates a new structure with the default values.
    */
    NetworkErrorStruct();

    /*!
        Creates an network error of the given type.
    */
    NetworkErrorStruct(int error);

    /*!
        Creates a network error with the given type and a human readable
        message.
    */
    NetworkErrorStruct(int code, QString phrase);

    /*!
        Copy constructor.
    */
    NetworkErrorStruct(const NetworkErrorStruct& other);

    /*!
        Assign operator.
    */
    NetworkErrorStruct& operator=(const NetworkErrorStruct& other);

    /*!
        \internal
    */
    friend QDBusArgument &operator<<(QDBusArgument &argument,
        const NetworkErrorStruct& error);

    /*!
        \internal
    */
    friend const QDBusArgument &operator>>(const QDBusArgument &argument,
        NetworkErrorStruct& error);

    /*!
        \fn int getCode() const

        Returns the type of network error.
    */
    int getCode() const;

    /*!
        \fn QString getPhrase const()

        Returns a human readable message about the network error.
    */
    QString getPhrase() const;

 private:

    /*!
        \internal
    */
    int _code;

    /*!
        \internal
    */
    QString _phrase;
};

}  // Errors

}  // Transfers

}  // Ubuntu

#endif // NETWORK_ERROR_STRUCT_H
