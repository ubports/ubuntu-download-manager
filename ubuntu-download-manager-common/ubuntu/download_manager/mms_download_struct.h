/*
 * Copyright 2013 Canonical Ltd.
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

#ifndef DOWNLOADER_LIB_MMS_DOWNLOAD_STRUCT_H
#define DOWNLOADER_LIB_MMS_DOWNLOAD_STRUCT_H

#include <QDBusArgument>

namespace Ubuntu {

namespace DownloadManager {

class MmsDownloadStruct {
    Q_PROPERTY(QString url READ getUrl)
    Q_PROPERTY(QString hostName READ getHostName)
    Q_PROPERTY(quint16 port READ getPort)
    Q_PROPERTY(QString user READ getUser)
    Q_PROPERTY(QString password READ getPassword)
 public:
    MmsDownloadStruct();
    MmsDownloadStruct(const QString& url,
                      const QString& hostName,
                      int port,
                      const QString& user,
                      const QString& password);
    MmsDownloadStruct(const MmsDownloadStruct& other);
    MmsDownloadStruct& operator=(const MmsDownloadStruct& other);

    friend QDBusArgument &operator<<(QDBusArgument &argument,
        const MmsDownloadStruct& download);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument,
        MmsDownloadStruct& download);

    QString getUrl();
    QString getHostName();
    int getPort();
    QString getUser();
    QString getPassword();

 private:
    QString _url;
    QString _hostName;
    int _port;
    QString _user;
    QString _password;
};

}  // DownloadManager

}  // Ubuntu
#endif // MMS_DOWNLOAD_STRUCT_H
