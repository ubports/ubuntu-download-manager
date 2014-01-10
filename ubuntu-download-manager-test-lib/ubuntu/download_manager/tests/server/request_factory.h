/*
 * Copyright 2013-2014 Canonical Ltd.
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

#ifndef FAKE_QNETWORK_ACCESS_MANAGER_H
#define FAKE_QNETWORK_ACCESS_MANAGER_H

#include <QList>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPair>
#include <system/request_factory.h>
#include "ubuntu/download_manager/tests/fake.h"

using namespace Ubuntu::DownloadManager::System;

class RequestWrapper : public QObject {
    Q_OBJECT

 public:
    explicit RequestWrapper(const QNetworkRequest& request,
                            QObject* parent = 0);

    QNetworkRequest request();
 private:
    QNetworkRequest _request;
};

class FakeRequestFactory : public RequestFactory, public Fake {
    Q_OBJECT

 public:
    explicit FakeRequestFactory(QObject* parent = 0);

    // overriden methods used to fake the nam
    NetworkReply* get(const QNetworkRequest& request);
    QList<QSslCertificate> acceptedCertificates() override;
    void setAcceptedCertificates(const QList<QSslCertificate>& certs) override;
};

#endif  // FAKE_QNETWORK_ACCESS_MANAGER_H
