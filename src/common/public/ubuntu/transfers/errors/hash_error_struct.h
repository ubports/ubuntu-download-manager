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

#pragma once

#include <QString>

class QDBusArgument;

namespace Ubuntu {

namespace Transfers {

namespace Errors {

/*!
    \class HashErrorStruct
    \brief The HashErrorStruct represents the dbus structure that is used
           to communicate hash errors that happened in the download
           manager to the different clients.
    \since 0.9
*/
class HashErrorStruct {
    Q_PROPERTY(QString method READ getMethod)
    Q_PROPERTY(QString expected READ getExpected)
    Q_PROPERTY(QString checksum READ getChecksum)

 public:

    /*!
        Creates a new structure with the default values.
    */
    HashErrorStruct();

    /*!
        Creates a new structure with the given method, expected result and found checksum.
    */
    HashErrorStruct(const QString& method, const QString& expected, const QString& found);

    /*!
        Copy constructor.
    */
    HashErrorStruct(const HashErrorStruct& other);

    /*!
        Assign operator.
    */
    HashErrorStruct& operator=(const HashErrorStruct& other);

    /*!
        \internal
    */
    friend QDBusArgument &operator<<(QDBusArgument &argument,
            const HashErrorStruct& error);

    /*!
        \internal
    */
    friend const QDBusArgument &operator>>(const QDBusArgument &argument,
            HashErrorStruct& error);

    // properties getters

    /*!
        \fn QString getMethod() const

        Returns the method that was used to calculate the checksum.
    */
    QString getMethod() const;

    /*!
        \fn QString getExpected() const

        Returns the checksum that was expected.
    */
    QString getExpected() const;

    /*!
        \fn QString getChecksum() const

        Returns the checksum of the completed download.
    */
    QString getChecksum() const;

 private:

    /*!
        \internal
    */
    QString _method = QString::null;

    /*!
        \internal
    */
    QString _expected = QString::null;

    /*!
        \internal
    */
    QString _found = QString::null;
};

}

}

}
