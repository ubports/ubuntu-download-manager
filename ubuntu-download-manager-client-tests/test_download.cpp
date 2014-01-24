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

#include <QSignalSpy>
#include "test_download.h"

TestDownload::TestDownload(QObject *parent)
    : LocalTreeTestCase("TestDownload", parent) {
}

void
TestDownload::init() {
    LocalTreeTestCase::init();
    _man = Manager::createSessionManager(daemonPath(), this);

    _metadata["my-string"] = "canonical";
    _metadata["your-string"] = "developer";
    QMap<QString, QString> headers;
    QUrl notPresentFile = largeFileUrl();
    qDebug() << "URL is" << notPresentFile;
    DownloadStruct downStruct(notPresentFile.toString(),
        _metadata, headers);

    _down = _man->createDownload(downStruct);
}

void
TestDownload::cleanup(){
    LocalTreeTestCase::cleanup();
    delete _down;
    delete _man;
}

void
TestDownload::testAllowMobileDownload_data() {
    QTest::addColumn<bool>("enabled");

    QTest::newRow("Enabled") << true;
    QTest::newRow("Disabled") << false;
}

void
TestDownload::testAllowMobileDownload() {
    QFETCH(bool, enabled);
    _down->allowMobileDownload(enabled);
    auto isEnabled = _down->isMobileDownloadAllowed();
    QCOMPARE(enabled, isEnabled);
}

void
TestDownload::testAllowMobileDownloadError() {
    returnDBusErrors(true);
    _down->allowMobileDownload(false);
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
}

void
TestDownload::testSetThrottle_data() {
    QTest::addColumn<qulonglong>("speed");

    QTest::newRow("First row") << 200ULL;
    QTest::newRow("Second row") << 1212ULL;
    QTest::newRow("Third row") << 998ULL;
    QTest::newRow("Last row") << 60ULL;
}

void
TestDownload::testSetThrottle() {
    QFETCH(qulonglong, speed);
    _down->setThrottle(speed);
    auto currentSpeed = _down->throttle();
    QCOMPARE(speed, currentSpeed);
}

void
TestDownload::testSetThrottleError() {
    returnDBusErrors(true);
    _down->setThrottle(0);
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
}

void
TestDownload::testMetadata() {
    auto currentMetadata = _down->metadata();
    foreach(auto key, _metadata.keys()) {
        QCOMPARE(currentMetadata[key], _metadata[key]);
    }
}

void
TestDownload::testMetadataError() {
    returnDBusErrors(true);
    _down->metadata();
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
}

void
TestDownload::testProgressError() {
    returnDBusErrors(true);
    _down->progress();
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
}

void
TestDownload::testTotalSizeError() {
    returnDBusErrors(true);
    _down->totalSize();
    QVERIFY(_down->isError());
    QVERIFY(_down->error() != nullptr);
}
