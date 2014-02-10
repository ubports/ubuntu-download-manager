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
#include "common.h"

class QDBusArgument;

namespace Ubuntu {

namespace DownloadManager {

class DOWNLOAD_MANAGER_EXPORT NetworkErrorStruct {
    Q_PROPERTY(int code READ getCode)
    Q_PROPERTY(QString phrase READ getPhrase)

 public:
    NetworkErrorStruct();
    NetworkErrorStruct(int error);
    NetworkErrorStruct(int code, QString phrase);
    NetworkErrorStruct(const NetworkErrorStruct& other);
    NetworkErrorStruct& operator=(const NetworkErrorStruct& other);

    friend QDBusArgument &operator<<(QDBusArgument &argument,
        const NetworkErrorStruct& error);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument,
        NetworkErrorStruct& error);

    // property getters
    int getCode();
    QString getPhrase();

 private:
    int _code;
    QString _phrase;
};

}  // DownloadManager

}  // Ubuntu

#endif // NETWORK_ERROR_STRUCT_H
