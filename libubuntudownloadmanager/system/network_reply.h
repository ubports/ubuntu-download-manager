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

#ifndef DOWNLOADER_LIB_NETWORK_REPLY_H
#define DOWNLOADER_LIB_NETWORK_REPLY_H

#include <QByteArray>
#include <QObject>
#include <QNetworkReply>
#include <QSslError>

namespace Ubuntu {

namespace DownloadManager {

namespace System {

class NetworkReplyPrivate;
class NetworkReply : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(NetworkReply)

 public:
    explicit NetworkReply(QNetworkReply* reply, QObject *parent = 0);

    virtual QByteArray readAll();
    virtual void abort();
    virtual void setReadBufferSize(uint size);
    virtual void setAcceptedCertificates(const QList<QSslCertificate>& certs);
    virtual bool canIgnoreSslErrors(const QList<QSslError>& errors);

 signals:
    // signals fowarded from the real reply object
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void error(QNetworkReply::NetworkError code);
    void finished();
    void sslErrors(const QList<QSslError>& errors);

 private:
    // use pimpl so that we can mantains ABI compatibility
    NetworkReplyPrivate* d_ptr;
};

}  // System

}  // DownloadManager

}  // Ubuntu

#endif  // DOWNLOADER_LIB_NETWORK_REPLY_H
