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

#ifndef DOWNLOAD_STATE_STRUCT_H
#define DOWNLOAD_STATE_STRUCT_H

#include <QString>
#include <QVariantMap>

class QDBusArgument;
namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {
class DownloadsDb;
}

/*!
    \class DownloadStateStruct
    \brief The DownloadStateStruct represents the dbus structure that is used
           to communicate the download manager the details of a past or
           current download.
    \since 1.1

    The DownloadStateStruct allows to get the basic information of a download that
    was created by an application in the download manager.
*/
class DownloadStateStruct {
    Q_PROPERTY(int state READ getState)
    Q_PROPERTY(QString url READ getUrl)
    Q_PROPERTY(QString filePath READ getFilePath)
    Q_PROPERTY(QString hash READ getHash)
    Q_PROPERTY(QVariantMap metadata READ metadata)

    friend class Ubuntu::DownloadManager::Daemon::DownloadsDb;

 public:

    /*
       Default constructor.
     */
    DownloadStateStruct();

    /*
       Copy constructor.
    */
    DownloadStateStruct(const DownloadStateStruct& other);

    /*
       Assign operator.
    */
    DownloadStateStruct& operator=(const DownloadStateStruct& other);

    /*
        \internal
    */
    friend QDBusArgument &operator<<(QDBusArgument &argument,  const DownloadStateStruct& download);

    /*
        \internal
    */
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, DownloadStateStruct& download);

    /*
       \fn int getState()

       Returns the state in which this download currently is.
    */
    int getState() const;

    /*
       \fn QString getUrl()

       Returns the url that points to the file that will be downloaded.
    */
    QString getUrl() const;

    /*
       \fn QString getFilePath()

       Returns the file path at which this download was downloaded.
    */
    QString getFilePath() const;

    /*
       \fn QString getHash()

       Returns the hash associated with this download.
    */
    QString getHash() const;

    /*
       \fn QVariantMap getMetadata()

       Returns the metadata associated with this download.
    */
    QVariantMap getMetadata() const;

    /*
       \fn bool isValid();

       Returns if the download state if valid.
    */
    bool isValid();

 protected:

    /*
        \internal
    */
    DownloadStateStruct(int state, const QString& url, const QString& hash=QString());

    /*
        \internal
    */
    DownloadStateStruct(int state, const QString& url, const QString& filePath, const QString& hash=QString());

    /*
        \internal
    */
    DownloadStateStruct(int state, const QString& url, const QString& filePath, const QString& hash=QString(), const QVariantMap& metadata=QVariantMap());

 private:

    /*
        \internal
    */
    int _state = -1;

    /*
        \internal
    */
    QString _url = QString();

    /*
        \internal
    */
    QString _filePath = QString();

    /*
        \internal
    */
    QString _hash = QString();

    /*
        \internal
    */
    QVariantMap _metadata = QVariantMap();
};

}

}

#endif
