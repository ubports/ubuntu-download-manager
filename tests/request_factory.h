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
#include <ubuntu/transfers/system/request_factory.h>
#include <gmock/gmock.h>


namespace Ubuntu {

namespace Transfers {

using namespace System;

namespace Tests {

class MockRequestFactory : public RequestFactory {
 public:
    explicit MockRequestFactory(QObject* parent = 0)
        : RequestFactory(parent) {}

    MOCK_METHOD1(get, NetworkReply*(const QNetworkRequest&));
    MOCK_METHOD0(acceptedCertificates, QList<QSslCertificate>());
    MOCK_METHOD1(setAcceptedCertificates,
        void(const QList<QSslCertificate>&));
};

}  // Ubuntu

}  // Transfers

}  // Tests

#endif  // FAKE_QNETWORK_ACCESS_MANAGER_H
