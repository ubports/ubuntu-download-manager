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

#include "./network_reply.h"

/*
 * PRIVATE IMPLEMENTATION
 */

class NetworkReplyPrivate {
    Q_DECLARE_PUBLIC(NetworkReply)

 public:
    explicit NetworkReplyPrivate(QNetworkReply* reply, NetworkReply* parent);

    // public methods used by other parts of the code
    QByteArray readAll();
    void abort();
    void setReadBufferSize(uint size);
    void setIgnoreSslErrors(QList<QSslError> expectedSslErrors);
    bool ignoreSslErrors();

 private:
    QList<QSslError> _sslErrors;
    QNetworkReply* _reply;
    NetworkReply* q_ptr;
};

NetworkReplyPrivate::NetworkReplyPrivate(QNetworkReply* reply,
                                         NetworkReply* parent)
    : _reply(reply),
      q_ptr(parent) {
    // connect to all the signals so that we foward them
    Q_Q(NetworkReply);
    if (_reply != NULL) {
        q->connect(_reply, SIGNAL(downloadProgress(qint64, qint64)),
            q, SIGNAL(downloadProgress(qint64, qint64)));
        q->connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            q, SIGNAL(error(QNetworkReply::NetworkError)));
        q->connect(_reply, SIGNAL(finished()),
            q, SIGNAL(finished()));
        q->connect(_reply, SIGNAL(sslErrors(const QList<QSslError>&)),
            q, SIGNAL(sslErrors(const QList<QSslError>&)));
    }
}

QByteArray
NetworkReplyPrivate::readAll() {
    return _reply->readAll();
}

void
NetworkReplyPrivate::abort() {
    _reply->abort();
}

void
NetworkReplyPrivate::setReadBufferSize(uint size) {
    _reply->setReadBufferSize(size);
}

void
NetworkReplyPrivate::setIgnoreSslErrors(QList<QSslError> expectedSslErrors) {
    _sslErrors = expectedSslErrors;
}

bool
NetworkReplyPrivate::ignoreSslErrors() {
    if (_sslErrors.count() > 0) {
        _reply->ignoreSslErrors(_sslErrors);
        return true;
    }
    return false;
}

/*
 * PUBLIC IMPLEMENTATION
 */

NetworkReply::NetworkReply(QNetworkReply* reply, QObject *parent)
    : QObject(parent),
      d_ptr(new NetworkReplyPrivate(reply, this)) {
}


QByteArray
NetworkReply::readAll() {
    Q_D(NetworkReply);
    return d->readAll();
}

void
NetworkReply::abort() {
    Q_D(NetworkReply);
    d->abort();
}

void
NetworkReply::setReadBufferSize(uint size) {
    Q_D(NetworkReply);
    d->setReadBufferSize(size);
}

void
NetworkReply::setIgnoreSslErrors(QList<QSslError> expectedSslErrors) {
    Q_D(NetworkReply);
    d->setIgnoreSslErrors(expectedSslErrors);
}

bool
NetworkReply::ignoreSslErrors() {
    Q_D(NetworkReply);
    return d->ignoreSslErrors();
}

#include "moc_network_reply.cpp"
