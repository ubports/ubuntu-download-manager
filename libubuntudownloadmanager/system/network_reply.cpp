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

#include <QDebug>
#include "network_reply.h"

namespace Ubuntu {

namespace DownloadManager {

namespace System {

/*
 * PRIVATE IMPLEMENTATION
 */

class NetworkReplyPrivate {
    Q_DECLARE_PUBLIC(NetworkReply)

 public:
    NetworkReplyPrivate(QNetworkReply* reply, NetworkReply* parent)
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

    // public methods used by other parts of the code
    QByteArray readAll() {
        return _reply->readAll();
    }

    void abort() {
        _reply->abort();
    }

    void setReadBufferSize(uint size) {
        _reply->setReadBufferSize(size);
    }

    void setAcceptedCertificates(const QList<QSslCertificate>& certs) {
        _certs = certs;
        // build possible errors
        foreach(const QSslCertificate& certificate, _certs) {
            QSslError error(QSslError::SelfSignedCertificate, certificate);
            _sslErrors.append(error);
        }
    }

    bool canIgnoreSslErrors(const QList<QSslError>& errors) {
        if (_sslErrors.count() > 0 && errors.count() > 0) {
            foreach(QSslError error, errors) {
                QSslError::SslError type = error.error();
                if (type != QSslError::NoError &&
                    type != QSslError::SelfSignedCertificate) {
                    // we only support self signed certificates all errors
                    // will not be ignored
                    qDebug() << "SSL error type not ignored";
                    return false;
                } else if (type == QSslError::SelfSignedCertificate) {
                    // just ignore those errors of the added errors
                    if (!_certs.contains(error.certificate())) {
                        qDebug() << "SSL certificate not ignored";
                        return false;
                    }
                }
            }

            if (_reply != NULL) {
                _reply->ignoreSslErrors(_sslErrors);
            }

            return true;
        }
        return false;
    }

 private:
    QList<QSslCertificate> _certs;
    QList<QSslError> _sslErrors;
    QNetworkReply* _reply;
    NetworkReply* q_ptr;
};

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
NetworkReply::setAcceptedCertificates(const QList<QSslCertificate>& certs) {
    Q_D(NetworkReply);
    d->setAcceptedCertificates(certs);
}

bool
NetworkReply::canIgnoreSslErrors(const QList<QSslError>& errors) {
    Q_D(NetworkReply);
    return d->canIgnoreSslErrors(errors);
}

}  // System

}  // DownloadManager

}  // Ubuntu
