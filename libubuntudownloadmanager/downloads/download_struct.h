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

#ifndef DOWNLOADER_LIB_DOWNLOAD_STRUCT_H
#define DOWNLOADER_LIB_DOWNLOAD_STRUCT_H

#include <QDBusArgument>
#include <QMap>
#include <QString>
#include <QVariantMap>

typedef QMap<QString, QString> StringList;
class DownloadStruct {
    Q_PROPERTY(QString url READ getUrl)
    Q_PROPERTY(QString hash READ getHash)
    Q_PROPERTY(QString algorithm READ getAlgorithm)
    Q_PROPERTY(QVariantMap metadata READ getMetadata)
    Q_PROPERTY(StringList headers READ getHeaders)

 public:
    DownloadStruct();
    DownloadStruct(const QString& url,
                   const QVariantMap& metadata,
                   const QMap<QString, QString>& headers);
    DownloadStruct(const QString& url,
                   const QString& hash,
                   const QString& algorithm,
                   const QVariantMap& metadata,
                   const QMap<QString, QString>& headers);
    DownloadStruct(const DownloadStruct& other);
    DownloadStruct& operator=(const DownloadStruct& other);
    ~DownloadStruct();

    friend QDBusArgument &operator<<(QDBusArgument &argument,
        const DownloadStruct& download);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument,
        DownloadStruct& download);

    // properties getters
    QString getUrl();
    QString getHash();
    QString getAlgorithm();
    QVariantMap getMetadata();
    QMap<QString, QString> getHeaders();

 private:
    QString _url;
    QString _hash;
    QString _algorithm;
    QVariantMap _metadata;
    QMap<QString, QString> _headers;
};

#endif  // DOWNLOADER_LIB_DOWNLOAD_STRUCT_H
