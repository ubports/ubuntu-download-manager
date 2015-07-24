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

#include <QString>

class QDBusArgument;

namespace Ubuntu {

namespace Transfers {

namespace Errors {

/*!
    \class ProcessErrorStruct
    \brief The ProcessErrorStruct represents the dbus structure that is used
           to communicate process errors that happened in the download
           manager to the different clients.
    \since 0.3
*/
class ProcessErrorStruct {
    Q_PROPERTY(int code READ getCode)
    Q_PROPERTY(int exitCode READ getExitCode)
    Q_PROPERTY(QString phrase READ getPhrase)
    Q_PROPERTY(QString READ getStandardOutput)
    Q_PROPERTY(QString READ getStandardError)

 public:

    /*!
        Creates a new structure with the default values.
    */
    ProcessErrorStruct();

    /*!
        Create a new structure with the given error \a code and
        a given human readable message.
    */
    ProcessErrorStruct(int code, const QString& phrase);

    /*!
        Creates a new structure with the given error \a code and with
        the exit details of the process. The process details are its
        \a exitCode as well as the standard out via \a standardOut and
        standard error via \a standarError
    */
    ProcessErrorStruct(int code,
                       int exitCode,
                       const QString& standardOutput,
                       const QString& standardError);

    /*!
        Creates a new structure with the given error \a code , a human
        readable message and with the exit details of the process.
        The process details are its \a exitCode as well as the standard
        out via \a standardOut and standard error via \a standarError
    */
    ProcessErrorStruct(int code,
                       const QString& phrase,
                       int exitCode,
                       const QString& standardOutput,
                       const QString& standardError);

    /*!
        Copy constructor.
    */
    ProcessErrorStruct(const ProcessErrorStruct& other);

    /*!
        Assign operator.
    */
    ProcessErrorStruct& operator=(const ProcessErrorStruct& other);

    /*!
        \internal
    */
    friend QDBusArgument &operator<<(QDBusArgument &argument,
        const ProcessErrorStruct& error);

    /*!
        \internal
    */
    friend const QDBusArgument &operator>>(const QDBusArgument &argument,
        ProcessErrorStruct& error);

    /*!
        \fn int getCode() const

        Returns the error code of the process.
    */
    int getCode() const;

    /*!
        \fn int getExitCode() const

        Returns the exit code of the process.
    */
    int getExitCode() const;

    /*!
        \fn QString getPhrase() const

        Returns a human readable message of the occurred error.
    */
    QString getPhrase() const;

    /*!
        \fn QString getStandardOutput() const

        Returns the standard output of the process.
    */
    QString getStandardOutput() const;

    /*!
        \fn QString getStandardError() const

        Returns the standard error of the process.
    */
    QString getStandardError() const;

 private:

    /*!
        \internal
    */
    int _code;

    /*!
        \internal
    */
    int _exitCode;

    /*!
        \internal
    */
    QString _phrase;

    /*!
        \internal
    */
    QString _stdout;

    /*!
        \internal
    */
    QString _stderr;
};

}  // Errors

}  // Transfers

}  // Ubuntu
