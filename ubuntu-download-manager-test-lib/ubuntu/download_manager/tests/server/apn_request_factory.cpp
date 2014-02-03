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

#include "apn_request_factory.h"

FakeApnRequestFactory::FakeApnRequestFactory(const QNetworkProxy& proxy,
                                             bool stoppable,
                                             QObject* parent)
    : ApnRequestFactory(proxy, stoppable, parent) {
}

QString
FakeApnRequestFactory::proxyHost() {
    return _nam->proxy().hostName();
}

int
FakeApnRequestFactory::proxyPort() {
    return _nam->proxy().port();
}

QString
FakeApnRequestFactory::proxyUsername() {
    return _nam->proxy().user();
}

QString
FakeApnRequestFactory::proxyPassword() {
    return _nam->proxy().password();
}

QNetworkProxy::ProxyType
FakeApnRequestFactory::proxyType() {
    return _nam->proxy().type();
}
