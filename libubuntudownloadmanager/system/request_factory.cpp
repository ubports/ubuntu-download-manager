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
#include <QNetworkAccessManager>
#include <QSslError>
#include "request_factory.h"

namespace Ubuntu {

namespace DownloadManager {

namespace System {

/*
 * PRIVATE IMPLEMENTATION
 */

class RequestFactoryPrivate {
    Q_DECLARE_PUBLIC(RequestFactory)

 public:
    RequestFactoryPrivate(bool stoppable, RequestFactory* parent)
        : _stoppable(stoppable),
        q_ptr(parent) {
        _nam = new QNetworkAccessManager();
    }

    NetworkReply* get(const QNetworkRequest& request) {
        Q_Q(RequestFactory);

        QNetworkReply* qreply = _nam->get(request);
        NetworkReply* reply = new NetworkReply(qreply);

        if (_certs.count() > 0) {
            reply->setAcceptedCertificates(_certs);
        }

        if (_stoppable) {
            // if the daemon was started as stoppable it means that
            // we are in testing mode and we do not want to keep
            // the connections for too long
            _replies.append(reply);

            q->connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                q, SLOT(onError(QNetworkReply::NetworkError)));
            q->connect(reply, SIGNAL(finished()),
                q, SLOT(onFinished()));
            q->connect(reply, SIGNAL(sslErrors(const QList<QSslError>&)),
                q, SLOT(onSslErrors(const QList<QSslError>&)));
        }
        return reply;
    }

    QList<QSslCertificate> acceptedCertificates() {
        return _certs;
    }

    void setAcceptedCertificates(const QList<QSslCertificate>& certs) {
        _certs = certs;
    }

    void removeNetworkReply(NetworkReply* reply) {
        if (_replies.contains(reply)) {
            _replies.removeAll(reply);
            // stoppable is not really needed but is better check
            if (_stoppable && _replies.count() == 0) {
                qDebug() << "Clearing the connections cache.";
                _nam->clearAccessCache();
            }
        }
    }

    void onError(QNetworkReply::NetworkError) {
        Q_Q(RequestFactory);
        NetworkReply* sender = qobject_cast<NetworkReply*>(q->sender());
        removeNetworkReply(sender);
    }

    void onFinished() {
        Q_Q(RequestFactory);
        NetworkReply* sender = qobject_cast<NetworkReply*>(q->sender());
        removeNetworkReply(sender);
    }

    void onSslErrors(const QList<QSslError>& errors) {
        Q_Q(RequestFactory);
        NetworkReply* sender = qobject_cast<NetworkReply*>(q->sender());
        // only remove the connection and clear the cache if we cannot
        // ignore the ssl errors!

        foreach(QSslError error, errors) {
            QSslError::SslError type = error.error();
            if (type != QSslError::NoError &&
                type != QSslError::SelfSignedCertificate) {
                // we only support self signed certificates all errors
                // will not be ignored
                qDebug() << "SSL error type not ignored clearing cache";
                removeNetworkReply(sender);
            } else if (type == QSslError::SelfSignedCertificate) {
                // just ignore those errors of the added errors
                if (!_certs.contains(error.certificate())) {
                    qDebug() << "SSL certificate not ignored clearing cache";
                    removeNetworkReply(sender);
                }
            }
       }

    }

 private:
    bool _stoppable = false;
    QList<NetworkReply*> _replies;
    QList<QSslCertificate> _certs;
    QNetworkAccessManager* _nam;
    RequestFactory* q_ptr;
};

/*
 * PUBLIC IMPLEMENTATION
 */

RequestFactory::RequestFactory(bool stoppable, QObject *parent)
    : QObject(parent),
      d_ptr(new RequestFactoryPrivate(stoppable, this)) {
}

NetworkReply*
RequestFactory::get(const QNetworkRequest& request) {
    Q_D(RequestFactory);
    return d->get(request);
}

QList<QSslCertificate>
RequestFactory::acceptedCertificates() {
    Q_D(RequestFactory);
    return d->acceptedCertificates();
}

void
RequestFactory::setAcceptedCertificates(const QList<QSslCertificate>& certs) {
    Q_D(RequestFactory);
    d->setAcceptedCertificates(certs);
}

}  // System

}  // DownloadManager

}  // Ubuntu

#include "moc_request_factory.cpp"
