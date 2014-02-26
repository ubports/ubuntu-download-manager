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

#ifndef DOWNLOADER_LIB_PROCESS_ERROR_STRUCT_H
#define DOWNLOADER_LIB_PROCESS_ERROR_STRUCT_H

#include <QString>

class QDBusArgument;

namespace Ubuntu {

namespace Errors {

class ProcessErrorStruct {
    Q_PROPERTY(int code READ getCode)
    Q_PROPERTY(int exitCode READ getExitCode)
    Q_PROPERTY(QString phrase READ getPhrase)
    Q_PROPERTY(QString READ getStandardOutput)
    Q_PROPERTY(QString READ getStandardError)

 public:
    ProcessErrorStruct();
    ProcessErrorStruct(int code, QString phrase);
    ProcessErrorStruct(int code,
                       int exitCode,
                       QString standardOutput,
                       QString standardError);
    ProcessErrorStruct(int code,
                       QString phrase,
                       int exitCode,
                       QString standardOutput,
                       QString standardError);
    ProcessErrorStruct(const ProcessErrorStruct& other);
    ProcessErrorStruct& operator=(const ProcessErrorStruct& other);

    friend QDBusArgument &operator<<(QDBusArgument &argument,
        const ProcessErrorStruct& error);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument,
        ProcessErrorStruct& error);

    // property getters
    int getCode();
    int getExitCode();
    QString getPhrase();
    QString getStandardOutput();
    QString getStandardError();

 private:
    int _code;
    int _exitCode;
    QString _phrase;
    QString _stdout;
    QString _stderr;
};

}  // Errors

}  // Ubuntu

#endif // PROCESS_ERROR_STRUCT_H
