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

#include "network_reply.h"

/*
 * PRIVATE IMPLEMENTATION
 */

class NetworkReplyPrivate
{
    Q_DECLARE_PUBLIC(NetworkReply)
public:
    explicit NetworkReplyPrivate(QNetworkReply* reply, NetworkReply* parent);

    // public methods used by other parts of the code
    virtual QByteArray readAll();
    virtual void abort();

    // slots used to foward the reply signals
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onError(QNetworkReply::NetworkError code);
    void onFinished();
    void onSslErrors(const QList<QSslError>& errors);

private:
    QNetworkReply* _reply;
    NetworkReply* q_ptr;

};

NetworkReplyPrivate::NetworkReplyPrivate(QNetworkReply* reply, NetworkReply* parent) :
    _reply(reply),
    q_ptr(parent)
{
    // connect to all the signals so that we foward them
    Q_Q(NetworkReply);
    if (_reply != NULL)
    {
        q->connect(_reply, SIGNAL(downloadProgress(qint64, qint64)),
            q, SLOT(onDownloadProgress(qint64, qint64)));
        q->connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            q, SLOT(onError(QNetworkReply::NetworkError)));
        q->connect(_reply, SIGNAL(finished()),
            q, SLOT(onFinished()));
        q->connect(_reply, SIGNAL(sslErrors(const QList<QSslError>&)),
            q, SLOT(onSslErrors(const QList<QSslError>&)));
    }

}

QByteArray NetworkReplyPrivate::readAll()
{
    return _reply->readAll();
}

void NetworkReplyPrivate::abort()
{
    _reply->abort();
}

void NetworkReplyPrivate::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_Q(NetworkReply);
    emit q->downloadProgress(bytesReceived, bytesTotal);
}

void NetworkReplyPrivate::onError(QNetworkReply::NetworkError code)
{
    Q_Q(NetworkReply);
    emit q->error(code);
}

void NetworkReplyPrivate::onFinished()
{
    Q_Q(NetworkReply);
    emit q->finished();
}

void NetworkReplyPrivate::onSslErrors(const QList<QSslError>& errors)
{
    Q_Q(NetworkReply);
    emit q->sslErrors(errors);
}


/*
 * PUBLIC IMPLEMENTATION
 */

NetworkReply::NetworkReply(QNetworkReply* reply, QObject *parent) :
    QObject(parent),
    d_ptr(new NetworkReplyPrivate(reply, this))
{
}


QByteArray NetworkReply::readAll()
{
    Q_D(NetworkReply);
    return d->readAll();
}

void NetworkReply::abort()
{
    Q_D(NetworkReply);
    d->abort();
}

#include "moc_network_reply.cpp"
