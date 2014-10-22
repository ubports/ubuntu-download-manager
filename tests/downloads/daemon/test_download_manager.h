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

#ifndef TEST_DOWNLOADER_H
#define TEST_DOWNLOADER_H

#include <QObject>
#include <QSharedPointer>
#include <QTest>
#include <ubuntu/downloads/manager.h>
#include <ubuntu/downloads/file_download.h>

#include "apparmor.h"
#include "application.h"
#include "base_testcase.h"
#include "database.h"
#include "dbus_connection.h"
#include "dbus_proxy_factory.h"
#include "factory.h"
#include "queue.h"
#include "request_factory.h"
#include "system_network_info.h"
#include "uuid_factory.h"

using namespace Ubuntu::Transfers::Tests;
using namespace Ubuntu::DownloadManager;
using namespace Ubuntu::DownloadManager::Daemon;

class TestDownloadManager : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestDownloadManager(QObject *parent = 0)
        : BaseTestCase("TestDownloadManager", parent) {}

 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;

    // data functions
    void testCreateDownload_data();
    void testCreateDownloadWithHash_data();
    void testSetThrottleNotDownloads_data();
    void testSetThrottleWithDownloads_data();
    void testSizeChangedEmittedOnAddition_data();
    void testSizeChangedEmittedOnRemoval_data();

    // tests
    void testCreateDownload();
    void testCreateDownloadWithHash();
    void testSetThrottleNotDownloads();
    void testSetThrottleWithDownloads();
    void testSizeChangedEmittedOnAddition();
    void testSizeChangedEmittedOnRemoval();
    void testSetSelfSignedCerts();
    void testStoppable();
    void testNotStoppable();

    // all downloads tests
    void testGetAllDownloadsUnconfined();
    void testGetAllDownloadsConfined();
    void testAllDownloadsWithMetadataUnconfined();
    void testAllDownloadsWithMetadataConfined();

 private:
    void verifyMocks();
    QCryptographicHash::Algorithm algoFromString(const QString& data);

 private:
    MockApplication* _app;
    MockDatabase* _database;
    MockDBusConnection* _conn;
    MockDownloadFactory* _factory;
    MockDownloadQueue* _q;
    DownloadManager* _man;
    MockRequestFactory* _requestFactory;
    MockDBusProxyFactory* _dbusProxyFactory;
};

#endif  // TEST_DOWNLOADER_H
