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

#ifndef DOWNLOADER_LIB_GROUP_DOWNLOAD_STRUCT_H
#define DOWNLOADER_LIB_GROUP_DOWNLOAD_STRUCT_H

#include <QString>
#include "common.h"

class QDBusArgument;

namespace Ubuntu {

namespace DownloadManager {

/*!
    \class GroupDownloadStruct 
    \brief The GroupDownloadStruct represents a single download in a
           group download that will be created in the download manager.
    \since 0.3


    Group downloads can be viewed as atomic operations that perform the
    dowload of more than one file. This structure represents a single file
    ins such a type of download and therefore exposes a subset of the
    properties of a download.
*/
class DOWNLOAD_MANAGER_EXPORT GroupDownloadStruct {
    Q_PROPERTY(QString url READ getUrl)
    Q_PROPERTY(QString hash READ getHash)
    Q_PROPERTY(QString localFile READ getLocalFile)

 public:
    /*!
        Creates a new group download structure with the default values.
    */
    GroupDownloadStruct();

    /*!
        Creates a new group download structure that will download a file
        from \a url and will store it in the given \a localFile. The client
        can provided the \a hash against which the checksum will be
        performed.

        \note if \a hash is empty the download manager will not check the
              hash of the downloaded file.
    */
    GroupDownloadStruct(const QString& url, const QString& localFile,
        const QString& hash);

    /*!
        Copy constructor.
    */
    GroupDownloadStruct(const GroupDownloadStruct& other);

    /*!
        Assign operator.
    */
    GroupDownloadStruct& operator=(const GroupDownloadStruct& other);

    /*!
        \internal
    */
    friend QDBusArgument &operator<<(QDBusArgument &argument,
        const GroupDownloadStruct& group);

    /*!
        \internal
    */
    friend const QDBusArgument &operator>>(const QDBusArgument &argument,
        GroupDownloadStruct& group);

    /*!
        \fn getUrl() const

        Returns the url from which the download will be performed.
    */
    QString getUrl() const;

    /*!
        \fn QString getHash() const

        Returns the hash value against which the checksum will be performed.
    */
    QString getHash() const;

    /*!
        \fn QString getLocalFile() const

        Returns the local file where the download will be stored.
    */
    QString getLocalFile() const;

 private:

    /*!
        \internal
    */
    QString _url;

    /*!
        \internal
    */
    QString _localFile;

    /*!
        \internal
    */
    QString _hash;
};

}  // DownloadManager

}  // Ubuntu

Q_DECLARE_METATYPE(Ubuntu::DownloadManager::GroupDownloadStruct)

#endif  // DOWNLOADER_LIB_GROUP_DOWNLOAD_STRUCT_H
