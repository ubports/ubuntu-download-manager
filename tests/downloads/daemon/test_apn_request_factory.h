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

#ifndef TEST_APN_REQUEST_FACTORY_H
#define TEST_APN_REQUEST_FACTORY_H

#include <QObject>
#include <ubuntu/transfers/system/apn_request_factory.h>
#include "base_testcase.h"

using namespace Ubuntu::Transfers::System;

class PublicApnRequestFactory : public ApnRequestFactory {
 public:
    PublicApnRequestFactory(const QNetworkProxy& proxy,
                          bool stoppable = false,
                          QObject* parent=0)
       : ApnRequestFactory(proxy, stoppable, parent) { }


    QString proxyHost() {
        return _nam->proxy().hostName();
    }

    int proxyPort() {
        return _nam->proxy().port();
    }

    QString proxyUsername() {
        return _nam->proxy().user();
    }

    QString proxyPassword() {
        return _nam->proxy().password();
    }

    QNetworkProxy::ProxyType proxyType() {
        return _nam->proxy().type();
    }

};

class TestApnRequestFactory : public BaseTestCase {
    Q_OBJECT

public:
    explicit TestApnRequestFactory(QObject *parent = 0)
        : BaseTestCase("TestApnRequestFactory", parent) { }

 private slots:  // NOLINT(whitespace/indent)

    void testProxySet_data();
    void testProxySet();

};

#endif // TEST_APN_REQUEST_FACTORY_H
