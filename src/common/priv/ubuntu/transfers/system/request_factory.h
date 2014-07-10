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

#ifndef DOWNLOADER_LIB_REQUEST_FACTORY_H
#define DOWNLOADER_LIB_REQUEST_FACTORY_H

#include <QMutex>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QObject>
#include <QSslCertificate>
#include <QSslError>
#include <ubuntu/transfers/system/file_manager.h>
#include "network_reply.h"

namespace Ubuntu {

namespace Transfers {

namespace System {

class RequestFactory : public QObject {
    Q_OBJECT

 public:
    virtual NetworkReply* get(const QNetworkRequest& request);
    virtual NetworkReply* post(const QNetworkRequest& request, File* data);
    virtual NetworkReply* put(const QNetworkRequest& request, File* data);

    // mainly for testing purposes
    virtual QList<QSslCertificate> acceptedCertificates();
    virtual void setAcceptedCertificates(const QList<QSslCertificate>& certs);

    static RequestFactory* instance();
    static void setStoppable(bool stoppable);

    // only used for testing purposes
    static void setInstance(RequestFactory* instance);
    static void deleteInstance();

 protected:
    RequestFactory(bool stoppable = false, QObject *parent = 0);

 private:
    void removeNetworkReply(NetworkReply* reply);
    NetworkReply* buildRequest(QNetworkReply* qreply);

 private slots:
    void onError(QNetworkReply::NetworkError);
    void onFinished();
    void onSslErrors(const QList<QSslError>&);

 protected:
    QNetworkAccessManager* _nam;

 private:
    // used for the singleton
    static RequestFactory* _instance;
    static QMutex _mutex;
    static bool _isStoppable;

    // instance vars
    bool _stoppable = false;
    QList<NetworkReply*> _replies;
    QList<QSslCertificate> _certs;
};

}  // System

}  // Transfers

}  // Ubuntu

#endif  // DOWNLOADER_LIB_REQUEST_FACTORY_H
