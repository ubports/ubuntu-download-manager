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
       \fn QString getUrl()

       Returns the url that points to the file that will be downloaded.
    */
    int getState() const;

    /*
       \fn QString getUrl()

       Returns the url that points to the file that will be downloaded.
    */
    QString getUrl() const;

    /*
       \fn QString getUrl()

       Returns the url that points to the file that will be downloaded.
    */
    QString getFilePath() const;

    /*
       \fn QString getUrl()

       Returns the url that points to the file that will be downloaded.
    */
    QString getHash() const;

    /*
       \fn bool isValid();

       Returns if the download state if valid.
    */
    bool isValid();

 protected:

    /*
        \internal
    */
    DownloadStateStruct(int state, const QString& url, const QString& hash=QString::null);

    /*
        \internal
    */
    DownloadStateStruct(int state, const QString& url, const QString& filePath, const QString& hash=QString::null);

 private:

    /*
        \internal
    */
    int _state = -1;

    /*
        \internal
    */
    QString _url = QString::null;

    /*
        \internal
    */
    QString _filePath = QString::null;

    /*
        \internal
    */
    QString _hash = QString::null;
};

}

}

#endif
