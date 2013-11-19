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

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QObject>
#include <QSslCertificate>
#include <QSslError>
#include "app-downloader-lib_global.h"
#include "network_reply.h"

namespace Ubuntu {

namespace DownloadManager {

namespace System {

class RequestFactory : public QObject {
    Q_OBJECT

 public:
    RequestFactory(bool stoppable = false, QObject *parent = 0);

    virtual NetworkReply* get(const QNetworkRequest& request);

    // mainly for testing purposes
    virtual QList<QSslCertificate> acceptedCertificates();
    virtual void setAcceptedCertificates(const QList<QSslCertificate>& certs);

 private:
    void removeNetworkReply(NetworkReply* reply);

 private slots:
    void onError(QNetworkReply::NetworkError);
    void onFinished();
    void onSslErrors(const QList<QSslError>&);

 private:
    bool _stoppable = false;
    QList<NetworkReply*> _replies;
    QList<QSslCertificate> _certs;
    QNetworkAccessManager* _nam;
};

}  // System

}  // DownloadManager

}  // Ubuntu

#endif  // DOWNLOADER_LIB_REQUEST_FACTORY_H
