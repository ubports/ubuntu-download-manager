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

#ifndef FAKE_APN_REQUEST_FACTORY_H
#define FAKE_APN_REQUEST_FACTORY_H

#include <QObject>
#include <system/apn_request_factory.h>
#include "ubuntu/download_manager/tests/fake.h"

using namespace  Ubuntu::DownloadManager::System;

class FakeApnRequestFactory : public ApnRequestFactory {
    Q_OBJECT

 public:
    FakeApnRequestFactory(const QNetworkProxy& proxy,
                          bool stoppable = false,
                          QObject* parent=0);

    QString proxyHost();
    int proxyPort();
    QString proxyUsername();
    QString proxyPassword();
    QNetworkProxy::ProxyType proxyType();
};

#endif // FAKE_APN_REQUEST_FACTORY_H
