/*
 * Copyright 2014 Canonical Ltd.
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

#ifndef FAKE_REPLY_H
#define FAKE_REPLY_H

#include <ubuntu/transfers/system/network_reply.h>
#include <gmock/gmock.h>

namespace Ubuntu {

namespace Transfers {

using namespace System;

namespace Tests {

class MockNetworkReply : public NetworkReply {
 public:
    explicit MockNetworkReply(QObject *parent = 0)
        : NetworkReply(nullptr, parent) {}

    MOCK_METHOD0(readAll, QByteArray());
    MOCK_METHOD0(abort, void());
    MOCK_METHOD1(setReadBufferSize, void(uint size));
    MOCK_METHOD1(setAcceptedCertificates,
        void(const QList<QSslCertificate>&));
    MOCK_METHOD1(canIgnoreSslErrors, bool(const QList<QSslError>&));
    MOCK_CONST_METHOD1(attribute,
        QVariant(QNetworkRequest::Attribute code));
    MOCK_CONST_METHOD0(errorString, QString());

    using NetworkReply::downloadProgress;
    using NetworkReply::error;
    using NetworkReply::finished;
    using NetworkReply::sslErrors;
};

}  // Tests

}  // Transfers

}  // Ubuntu

#endif
