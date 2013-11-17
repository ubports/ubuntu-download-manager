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

#ifndef DOWNLOADER_LIB_SM_FILE_DOWNLOAD_H
#define DOWNLOADER_LIB_SM_FILE_DOWNLOAD_H

#include <QNetworkReply>
#include <QObject>
#include <QSslError>
#include "download.h"

namespace Ubuntu {

namespace DownloadManager {

class SMFileDownload : public QObject {
    Q_OBJECT

 public:
    explicit SMFileDownload(QObject *parent = 0);
    
    virtual void emitError(QString error);
    virtual void emitNetworkError(QNetworkReply::NetworkError code);
    virtual void emitSslError(const QList<QSslError>& errors);

    virtual void setState(Download::State state);
    virtual void requestDownload();

 signals:
    // the following signals are used to control the move between
    // states in the state machine

    // signal raised when we are done with a head request done to get as
    // much info as possible about the download
    void headRequestCompleted();

    // network related signals
    void error(QNetworkReply::NetworkError code);
    void sslErrors(const QList<QSslError>& errors);

    // raised when the connection is not available (either no connection
    // or we cannot use the connection, eg. WIFI only)
    void connectionEnabled();
    void connectionDisabled();

    // signals that are raised when performing download operations
    void downloadingStarted();
    void paused();
    void completed();
    void hashingStarted();
    void hashingError();
    void postProcessingStarted();
    void postProcessingError();
    void finished();
    void canceled();

};

}  // DownloadManager

}  // Ubuntu

#endif // SM_FILE_DOWNLOAD_H
