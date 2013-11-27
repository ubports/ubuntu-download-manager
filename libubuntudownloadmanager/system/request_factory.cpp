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
#include "request_factory.h"

namespace Ubuntu {

namespace DownloadManager {

namespace System {

RequestFactory* RequestFactory::_instance = NULL;
bool RequestFactory::_isStoppable = false;
QMutex RequestFactory::_mutex;

RequestFactory::RequestFactory(bool stoppable, QObject* parent)
    : QObject(parent),
      _stoppable(stoppable) {
    _nam = new QNetworkAccessManager(this);
}

NetworkReply*
RequestFactory::get(const QNetworkRequest& request) {
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

        connect(reply, &NetworkReply::error,
            this, &RequestFactory::onError);
        connect(reply, &NetworkReply::finished,
            this, &RequestFactory::onFinished);
        connect(reply, &NetworkReply::sslErrors,
            this, &RequestFactory::onSslErrors);
    }
    return reply;
}

QList<QSslCertificate>
RequestFactory::acceptedCertificates() {
    return _certs;
}

void
RequestFactory::setAcceptedCertificates(const QList<QSslCertificate>& certs) {
    _certs = certs;
}

RequestFactory*
RequestFactory::instance() {
    if(_instance == NULL) {
        _mutex.lock();
        if(_instance == NULL)
            _instance = new RequestFactory(_isStoppable);
        _mutex.unlock();
    }
    return _instance;
}

void
RequestFactory::setStoppable(bool stoppable) {
    _isStoppable = stoppable;
}

void
RequestFactory::setInstance(RequestFactory* instance) {
    _instance = instance;
}

void
RequestFactory::removeNetworkReply(NetworkReply* reply) {
    if (_replies.contains(reply)) {
        _replies.removeAll(reply);
        // stoppable is not really needed but is better check
        if (_stoppable && _replies.count() == 0) {
            qDebug() << "Clearing the connections cache.";
            _nam->clearAccessCache();
        }
    }
}

void
RequestFactory::onError(QNetworkReply::NetworkError) {
    NetworkReply* senderObj = qobject_cast<NetworkReply*>(sender());
    removeNetworkReply(senderObj);
}

void
RequestFactory::onFinished() {
    NetworkReply* senderObj = qobject_cast<NetworkReply*>(sender());
    removeNetworkReply(senderObj);
}

void
RequestFactory::onSslErrors(const QList<QSslError>& errors) {
    NetworkReply* senderObj = qobject_cast<NetworkReply*>(sender());
    // only remove the connection and clear the cache if we cannot
    // ignore the ssl errors!

    foreach(QSslError error, errors) {
        QSslError::SslError type = error.error();
        if (type != QSslError::NoError &&
            type != QSslError::SelfSignedCertificate) {
            // we only support self signed certificates all errors
            // will not be ignored
            qDebug() << "SSL error type not ignored clearing cache";
            removeNetworkReply(senderObj);
        } else if (type == QSslError::SelfSignedCertificate) {
            // just ignore those errors of the added errors
            if (!_certs.contains(error.certificate())) {
                qDebug() << "SSL certificate not ignored clearing cache";
                removeNetworkReply(senderObj);
            }
        }
   }

}


}  // System

}  // DownloadManager

}  // Ubuntu
