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

#include <QScopedPointer>
#include <ubuntu/transfers/tests/system/apn_request_factory.h>
#include "test_apn_request_factory.h"

TestApnRequestFactory::TestApnRequestFactory(QObject* parent)
    : BaseTestCase("TestApnRequestFactory", parent) {
}

void
TestApnRequestFactory::testProxySet_data() {
    QTest::addColumn<QString>("hostName");
    QTest::addColumn<int>("port");
    QTest::addColumn<QString>("username");
    QTest::addColumn<QString>("password");

    QTest::newRow("First row") << "http://proxy.com" << 88
        << "mandel" << "Qwsxd";
    QTest::newRow("Second row") << "http://ubuntu.com" << 99
        << "mark" << "trusty";
    QTest::newRow("Third row") << "http://one.ubuntu.com" << 200
        << "one" << "drop";
}

void
TestApnRequestFactory::testProxySet() {
    QFETCH(QString, hostName);
    QFETCH(int, port);
    QFETCH(QString, username);
    QFETCH(QString, password);

    QNetworkProxy proxy(QNetworkProxy::HttpProxy, hostName, port,
        username, password);
    QScopedPointer<FakeApnRequestFactory> nam(new FakeApnRequestFactory(proxy));

    QCOMPARE(hostName, nam->proxyHost());
    QCOMPARE(port, nam->proxyPort());
    QCOMPARE(username, nam->proxyUsername());
    QCOMPARE(password, nam->proxyPassword());
}
