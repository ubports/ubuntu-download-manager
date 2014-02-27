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
#include <QVariant>
#include <QSslError>

namespace Ubuntu {

namespace Transfers {

namespace System {

class NetworkReply : public QObject {
    Q_OBJECT

 public:
    NetworkReply(QNetworkReply* reply, QObject *parent = 0);
    virtual ~NetworkReply();

    virtual QByteArray readAll();
    virtual void abort();
    virtual void setReadBufferSize(uint size);
    virtual void setAcceptedCertificates(const QList<QSslCertificate>& certs);
    virtual bool canIgnoreSslErrors(const QList<QSslError>& errors);
    virtual QVariant attribute(QNetworkRequest::Attribute code) const;
    virtual QString errorString() const;

 signals:
    // signals forwarded from the real reply object
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void error(QNetworkReply::NetworkError code);
    void finished();
    void sslErrors(const QList<QSslError>& errors);

 private:
    QList<QSslCertificate> _certs;
    QList<QSslError> _sslErrors;
    QNetworkReply* _reply;
};

}  // System

}  // Transfers

}  // Ubuntu

#endif  // DOWNLOADER_LIB_NETWORK_REPLY_H
