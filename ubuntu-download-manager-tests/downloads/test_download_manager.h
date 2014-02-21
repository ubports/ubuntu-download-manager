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
#include <ubuntu/downloads/manager.h>
#include <ubuntu/downloads/file_download.h>
#include <ubuntu/download_manager/tests/server/application.h>
#include <ubuntu/download_manager/tests/server/apparmor.h>
#include <ubuntu/download_manager/tests/server/dbus_connection.h>
#include <ubuntu/download_manager/tests/server/queue.h>
#include <ubuntu/download_manager/tests/server/factory.h>
#include <ubuntu/download_manager/tests/server/request_factory.h>
#include <ubuntu/download_manager/tests/server/uuid_factory.h>
#include <ubuntu/download_manager/tests/server/system_network_info.h>
#include <ubuntu/download_manager/tests/test_runner.h>
#include <ubuntu/download_manager/tests/base_testcase.h>

using namespace Ubuntu::DownloadManager;
using namespace Ubuntu::DownloadManager::Daemon;

class TestDownloadManager : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestDownloadManager(QObject *parent = 0);

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
    void testGetAllDownloads();
    void testAllDownloadsWithMetadata();
    void testSetThrottleNotDownloads();
    void testSetThrottleWithDownloads();
    void testSizeChangedEmittedOnAddition();
    void testSizeChangedEmittedOnRemoval();
    void testSetSelfSignedCerts();
    void testStoppable();
    void testNotStoppable();

 private:
    QCryptographicHash::Algorithm algoFromString(const QString& data);

 private:
    FakeApplication* _app;
    FakeSystemNetworkInfo* _networkInfo;
    FakeRequestFactory* _requestFactory;
    FakeDownloadFactory* _downloadFactory;
    FakeDBusConnection* _conn;
    FakeDownloadQueue* _q;
    QSharedPointer<FakeUuidFactory> _uuidFactory;
    FakeAppArmor* _apparmor;
    Manager* _man;
};

DECLARE_TEST(TestDownloadManager)

#endif  // TEST_DOWNLOADER_H
