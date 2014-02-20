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

#include <QDebug>
#include <QSignalSpy>
#include <ubuntu/download_manager/download.h>
#include "test_manager.h"

TestManager::TestManager(QObject *parent)
    : LocalTreeTestCase("TestManager", parent) {
}

void
TestManager::init() {
    LocalTreeTestCase::init();
    _man = Manager::createSessionManager(daemonPath(), this);
}

void
TestManager::cleanup() {
    LocalTreeTestCase::cleanup();
    delete _man;
}

void
TestManager::testAllowMobileDataDownload_data() {
    QTest::addColumn<bool>("enabled");

    QTest::newRow("Enabled") << true;
    QTest::newRow("Disabled") << false;
}

void
TestManager::testAllowMobileDataDownload() {
    QFETCH(bool, enabled);
    _man->allowMobileDataDownload(enabled);
    QCOMPARE(enabled, _man->isMobileDataDownload());
}

void
TestManager::testAllowMobileDataDownloadError() {
    returnDBusErrors(true);
    _man->allowMobileDataDownload(true);
    QVERIFY(_man->isError());
}

void
TestManager::testIsMobileDataDownloadError() {
    returnDBusErrors(true);
    _man->isMobileDataDownload();
    QVERIFY(_man->isError());
}

void
TestManager::testDefaultThrottleError() {
    returnDBusErrors(true);
    _man->defaultThrottle();
    QVERIFY(_man->isError());
}

void
TestManager::testSetDefaultThrottle_data() {
    QTest::addColumn<uint>("speed");

    QTest::newRow("First row") << 200u;
    QTest::newRow("Second row") << 1212u;
    QTest::newRow("Third row") << 998u;
    QTest::newRow("Last row") << 60u;
}

void
TestManager::testSetDefaultThrottle() {
    QFETCH(uint, speed);
    _man->setDefaultThrottle(speed);
    QCOMPARE(speed, (uint)_man->defaultThrottle());
}

void
TestManager::testSetDefaultThrottleError() {
    returnDBusErrors(true);
    _man->setDefaultThrottle(30);
    QVERIFY(_man->isError());
}

void
TestManager::testExitError() {
    returnDBusErrors(true);
    _man->exit();
    QVERIFY(_man->isError());
}

void
TestManager::testCreateDownloadSignalsEmitted() {
    QString url = "http://example.com";
    QVariantMap metadata;
    QMap<QString, QString> headers;

    DownloadStruct downStruct(url, metadata, headers);

    QSignalSpy managerSpy(_man, SIGNAL(downloadCreated(Download*)));
    _man->createDownload(downStruct);
    QTRY_COMPARE(1, managerSpy.count());
    auto down = managerSpy.takeFirst().at(0).value<Download*>();
    delete down;
}

void
TestManager::testCreateDownloadSignalsEmittedCallbacks() {
    QString url = "http://example.com";
    QVariantMap metadata;
    QMap<QString, QString> headers;

    DownloadCb cb = [](Download*){};
    DownloadStruct downStruct(url, metadata, headers);

    QSignalSpy managerSpy(_man, SIGNAL(downloadCreated(Download*)));
    _man->createDownload(downStruct, cb, cb);
    QTRY_COMPARE(1, managerSpy.count());
    auto down = managerSpy.takeFirst().at(0).value<Download*>();
    delete down;
}

void
TestManager::testGetAllDownloadsSignalsEmitted_data() {
    QTest::addColumn<int>("count");

    QTest::newRow("5 downloads") << 5;
    QTest::newRow("10 downloads") << 10;
    QTest::newRow("15 downloads") << 15;
}

void
TestManager::testGetAllDownloadsSignalsEmitted() {
    QFETCH(int, count);

    QString url = "http://example.com/";
    QVariantMap metadata;
    QMap<QString, QString> headers;

    QSignalSpy managerSpy(_man, SIGNAL(downloadCreated(Download*)));
    QSignalSpy listSpy(_man, SIGNAL(downloadsFound(DownloadsList*)));

    for (int index = 0; index < count; index++) {
        url += index;
        DownloadStruct downStruct(url, metadata, headers);
        _man->createDownload(downStruct);
    }

    // ensure that all of the are created
    QTRY_COMPARE(count, managerSpy.count());
    _man->getAllDownloads();

    QTRY_COMPARE(1, listSpy.count());
    auto downs = listSpy.takeFirst().at(0).value<DownloadsList*>();
    QCOMPARE(count, downs->downloads().count());

    delete downs;
}

void
TestManager::testGetAllDownloadsSignalsEmittedCallbacks_data() {
    QTest::addColumn<int>("count");

    QTest::newRow("5 downloads") << 5;
    QTest::newRow("10 downloads") << 10;
    QTest::newRow("15 downloads") << 15;
}

void
TestManager::testGetAllDownloadsSignalsEmittedCallbacks() {
    QFETCH(int, count);

    QString url = "http://example.com/";
    QVariantMap metadata;
    QMap<QString, QString> headers;
    DownloadsListCb cb = [](DownloadsList*){};

    QSignalSpy managerSpy(_man, SIGNAL(downloadCreated(Download*)));
    QSignalSpy listSpy(_man, SIGNAL(downloadsFound(DownloadsList*)));

    for (int index = 0; index < count; index++) {
        url += index;
        DownloadStruct downStruct(url, metadata, headers);
        _man->createDownload(downStruct);
    }

    // ensure that all of the are created
    QTRY_COMPARE(count, managerSpy.count());
    _man->getAllDownloads(cb, cb);

    QTRY_COMPARE(1, listSpy.count());
    auto downs = listSpy.takeFirst().at(0).value<DownloadsList*>();
    QCOMPARE(count, downs->downloads().count());

    delete downs;
}

void
TestManager::testGetAllDownloadsMetadataSignalsEmitted_data() {
    QTest::addColumn<int>("count");

    QTest::newRow("5 downloads") << 5;
    QTest::newRow("10 downloads") << 10;
    QTest::newRow("15 downloads") << 15;
}

void
TestManager::testGetAllDownloadsMetadataSignalsEmitted() {
    QFETCH(int, count);
    int metadataCount = 5;
    QString key = "test";
    QString value = "metadata";

    QString url = "http://example.com/";
    QVariantMap metadata;
    QMap<QString, QString> headers;

    QSignalSpy managerSpy(_man, SIGNAL(downloadCreated(Download*)));
    QSignalSpy listSpy(_man, SIGNAL(
        downloadsWithMetadataFound(const QString&, const QString&,
            DownloadsList*)));

    for (int index = 0; index < count; index++) {
        url += index;
        DownloadStruct downStruct(url, metadata, headers);
        _man->createDownload(downStruct);
    }

    // add downloads with metadata
    metadata[key] = value;
    for (int index = count; index < metadataCount + count; index++) {
        url += index;
        DownloadStruct downStruct(url, metadata, headers);
        _man->createDownload(downStruct);
    }
    // ensure that all of the are created
    QTRY_COMPARE(count + metadataCount, managerSpy.count());
    _man->getAllDownloadsWithMetadata(key, value);

    QTRY_COMPARE(1, listSpy.count());

    auto signalInfo = listSpy.takeFirst();
    auto signalKey = signalInfo.at(0).toString();
    QCOMPARE(key, signalKey);

    auto signalValue = signalInfo.at(1).toString();
    QCOMPARE(value, signalValue);

    auto downs = signalInfo.at(2).value<DownloadsList*>();

    QCOMPARE(metadataCount, downs->downloads().count());

    delete downs;
}

void
TestManager::testGetAllDownloadsMetadataSignalsEmittedCallbacks_data() {
    QTest::addColumn<int>("count");

    QTest::newRow("5 downloads") << 5;
    QTest::newRow("10 downloads") << 10;
    QTest::newRow("15 downloads") << 15;
}

void
TestManager::testGetAllDownloadsMetadataSignalsEmittedCallbacks() {
    QFETCH(int, count);
    int metadataCount = 5;
    QString key = "test";
    QString value = "metadata";

    QString url = "http://example.com/";
    QVariantMap metadata;
    QMap<QString, QString> headers;
    MetadataDownloadsListCb cb =
        [](const QString&, const QString&, DownloadsList*){};

    QSignalSpy managerSpy(_man, SIGNAL(downloadCreated(Download*)));
    QSignalSpy listSpy(_man, SIGNAL(
        downloadsWithMetadataFound(const QString&, const QString&,
            DownloadsList*)));

    for (int index = 0; index < count; index++) {
        url += index;
        DownloadStruct downStruct(url, metadata, headers);
        _man->createDownload(downStruct);
    }

    // add downloads with metadata
    metadata[key] = value;
    for (int index = count; index < metadataCount + count; index++) {
        url += index;
        DownloadStruct downStruct(url, metadata, headers);
        _man->createDownload(downStruct);
    }
    // ensure that all of the are created
    QTRY_COMPARE(count + metadataCount, managerSpy.count());
    _man->getAllDownloadsWithMetadata(key, value, cb, cb);

    QTRY_COMPARE(1, listSpy.count());

    auto signalInfo = listSpy.takeFirst();
    auto signalKey = signalInfo.at(0).toString();
    QCOMPARE(key, signalKey);

    auto signalValue = signalInfo.at(1).toString();
    QCOMPARE(value, signalValue);

    auto downs = signalInfo.at(2).value<DownloadsList*>();

    QCOMPARE(metadataCount, downs->downloads().count());

    delete downs;
}
