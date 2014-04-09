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

#include <QMap>
#include <QString>
#include <QVariantMap>
#include "common.h"

class QDBusArgument;
namespace Ubuntu {

namespace DownloadManager {

typedef QMap<QString, QString> StringList;

/*!
    \class DownloadStruct 
    \brief The DownloadStruct represents the dbus structure that is used
           to communicate the download manager the details of a new
           download to be created.
    \since 0.3

    The DownloadStruct is the configuration structure that is sent to the
    download manager specifying the different attribute of a download. This
    attributes include from the url to be downloaded to the hash checksum
    and the algorithm used for the checksum.
*/
class DownloadStruct {
    Q_PROPERTY(QString url READ getUrl)
    Q_PROPERTY(QString hash READ getHash)
    Q_PROPERTY(QString algorithm READ getAlgorithm)
    Q_PROPERTY(QVariantMap metadata READ getMetadata)
    Q_PROPERTY(StringList headers READ getHeaders)

 public:
    /*
    */
    DownloadStruct();

    /*
       Creates a new download structure that will tell the download manager
       to download the file found in the provided \q url with no metadata,
       empty headers and without a checksum check.
    */
    DownloadStruct(const QString& url);

    /*
       Creates a new download structure that will tell the download manager
       to download the file found in the provided \a url with the given
       \a metadata and \a headers but without a checksum check.
    */
    DownloadStruct(const QString& url,
                   const QVariantMap& metadata,
                   const QMap<QString, QString>& headers);

    /*
       Creates a new download structure that will tell the download manager
       to download the file found in the provided \a url with the given
       \a metadata and \a headers. Once the file is download the checksum
       check will be done against the provided \a hash using the specified
       \a algorithm. The \a algorithm can be of one of the following string
       values:

         - "md5"
         - "sha1"
         - "sha224"
         - "sha256"
         - "sha384"
         - "sha512"

      \note If the hash provided is empty the download manager will assume
            that there is no need to perform the checksum.
      \note If the algorithm is an empty string or a not recognized value
            md5 will be used.
    */
    DownloadStruct(const QString& url,
                   const QString& hash,
                   const QString& algorithm,
                   const QVariantMap& metadata,
                   const QMap<QString, QString>& headers);

    /*
       Copy constructor.
    */
    DownloadStruct(const DownloadStruct& other);

    /*
       Assign operator.
    */
    DownloadStruct& operator=(const DownloadStruct& other);

    /*
        \internal
    */
    friend QDBusArgument &operator<<(QDBusArgument &argument,
        const DownloadStruct& download);

    /*
        \internal
    */
    friend const QDBusArgument &operator>>(const QDBusArgument &argument,
        DownloadStruct& download);

    /*
       \fn QString getUrl()

       Returns the url that points to the file that will be downloaded.
    */
    QString getUrl();

    /*
       \fn QString getHash()

       Returns the hash against which the checksum will be performed.
    */
    QString getHash();

    /*
       \fn QString getAlgorithm()

       Returns the algorithm that will be used to perform the checksum.
    */
    QString getAlgorithm();

    /*
       \fn QVariantMap getMetadata()

       Returns the metadata carried by the download.
    */
    QVariantMap getMetadata();

    /*
       \fn QMap<QString, QString> getHeaders()

       Returns the extra headers that will be used in the GET request of
       the download.
    */
    QMap<QString, QString> getHeaders();

 private:

    /*
        \internal
    */
    QString _url;

    /*
        \internal
    */
    QString _hash;

    /*
        \internal
    */
    QString _algorithm;

    /*
        \internal
    */
    QVariantMap _metadata;

    /*
        \internal
    */
    QMap<QString, QString> _headers;
};

}  // DownloadManager

}  // Ubunutu

#endif  // DOWNLOADER_LIB_DOWNLOAD_STRUCT_H
