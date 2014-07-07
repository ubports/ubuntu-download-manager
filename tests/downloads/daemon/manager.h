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

#ifndef FAKE_DOWNLOAD_MANAGER_H
#define FAKE_DOWNLOAD_MANAGER_H

#include <QObject>
#include <ubuntu/downloads/manager.h>
#include <ubuntu/transfers/manager_factory.h>
#include <gmock/gmock.h>

using namespace Ubuntu::Transfers;
using namespace Ubuntu::Transfers::Tests;
using namespace Ubuntu::System;
using namespace Ubuntu::DownloadManager::Daemon;

class MockDownloadManager : public DownloadManager {
 public:
    MockDownloadManager(Application* app,
                        DBusConnection* connection,
                        bool stoppable = false,
                        QObject *parent = 0)
        : DownloadManager(app, connection, stoppable, parent){}
    MockDownloadManager(Application* app,
                        DBusConnection* connection,
                        Factory* downloadFactory,
                        Queue* queue,
                        bool stoppable = false,
                        QObject *parent = 0)
        : DownloadManager(app, connection, downloadFactory, queue, stoppable,
                parent) {}

    MOCK_METHOD0(acceptedCertificates,
        QList<QSslCertificate>());
    MOCK_METHOD1(setAcceptedCertificates,
        void(const QList<QSslCertificate>&));

    using DownloadManager::sizeChanged;
};

class MockDownloadManagerFactory : public ManagerFactory {
 public:
    MOCK_METHOD4(createManager,
        BaseManager*(Application*, DBusConnection*, bool, QObject*));
};

#endif  // FAKE_DOWNLOAD_MANAGER_H
