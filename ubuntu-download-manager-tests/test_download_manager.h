/*
 * Copyright 2013 Canonical Ltd.
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
#include <download_manager.h>
#include <single_download.h>
#include "./fake_apparmor.h"
#include "./test_runner.h"
#include "./fake_dbus_connection.h"
#include "./fake_download_queue.h"
#include "./fake_download_factory.h"
#include "./fake_request_factory.h"
#include "./fake_uuid_factory.h"
#include "./fake_system_network_info.h"

class TestDownloadManager : public QObject {
    Q_OBJECT

 public:
    explicit TestDownloadManager(QObject *parent = 0);

 private slots:  // NOLINT(whitespace/indent)

    void init();
    void cleanup();

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

 private:
    QCryptographicHash::Algorithm algoFromString(const QString& data);

 private:
    FakeSystemNetworkInfo* _networkInfo;
    FakeRequestFactory* _requestFactory;
    FakeDownloadFactory* _downloadFactory;
    QSharedPointer<FakeDBusConnection> _conn;
    FakeDownloadQueue* _q;
    FakeUuidFactory* _uuidFactory;
    FakeAppArmor* _apparmor;
    DownloadManager* _man;
};

DECLARE_TEST(TestDownloadManager)

#endif  // TEST_DOWNLOADER_H
