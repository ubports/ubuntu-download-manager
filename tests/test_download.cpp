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

#include <QNetworkRequest>
#include <QSignalSpy>
#include <QSslError>
#include <ubuntu/download_manager/metatypes.h>
#include <ubuntu/transfers/system/hash_algorithm.h>
#include <ubuntu/transfers/system/uuid_utils.h>
#include "filename_mutex.h"
#include "matchers.h"
#include "network_reply.h"
#include "process.h"
#include "test_download.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::AnyNumber;
using ::testing::Return;
using ::testing::Return;
using ::testing::AnyOf;

using namespace Ubuntu::Transfers::Tests;
using namespace Ubuntu::Transfers::System;
using namespace Ubuntu::DownloadManager::Daemon;

void
TestDownload::init() {
    BaseTestCase::init();

    _id = UuidUtils::getDBusString(QUuid::createUuid());
    _isConfined = false;
    _rootPath = testDirectory();
    _path = "random path to dbus";
    _url = QUrl("http://ubuntu.com");
    _algo = "Sha256";
    _networkInfo = new MockSystemNetworkInfo();
    SystemNetworkInfo::setInstance(_networkInfo);
    _reqFactory = new MockRequestFactory();
    RequestFactory::setInstance(_reqFactory);
    _processFactory = new MockProcessFactory();
    ProcessFactory::setInstance(_processFactory);
    _fileManager = new MockFileManager();
    FileManager::setInstance(_fileManager);
    _cryptoFactory = new MockCryptographicHashFactory();
    CryptographicHashFactory::setInstance(_cryptoFactory);
}

void
TestDownload::verifyMocks() {
    QVERIFY(Mock::VerifyAndClearExpectations(_networkInfo));
    QVERIFY(Mock::VerifyAndClearExpectations(_reqFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(_processFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(_fileManager));
    QVERIFY(Mock::VerifyAndClearExpectations(_cryptoFactory));
}

void
TestDownload::cleanup() {
    BaseTestCase::cleanup();

    SystemNetworkInfo::deleteInstance();
    RequestFactory::deleteInstance();
    ProcessFactory::deleteInstance();
    FileManager::deleteInstance();
    FileNameMutex::deleteInstance();
    CryptographicHashFactory::deleteInstance();
}

void
TestDownload::testNoHashConstructor_data() {
    QTest::addColumn<QString>("id");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QUrl>("url");

    QTest::newRow("First row") << UuidUtils::getDBusString(QUuid::createUuid())
        << "/path/to/first/app" << QUrl("http://ubuntu.com");
    QTest::newRow("Second row") << UuidUtils::getDBusString(QUuid::createUuid())
        << "/path/to/second/app" << QUrl("http://ubuntu.com/juju");
    QTest::newRow("Third row") << UuidUtils::getDBusString(QUuid::createUuid())
        << "/path/to/third/app" << QUrl("http://ubuntu.com/tablet");
    QTest::newRow("Last row") << UuidUtils::getDBusString(QUuid::createUuid())
        << "/path/to/last/app" << QUrl("http://ubuntu.com/phone");
}

void
TestDownload::testNoHashConstructor() {
    QFETCH(QString, id);
    QFETCH(QString, path);
    QFETCH(QUrl, url);

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QScopedPointer<FileDownload> download(new FileDownload(id, path,
        _isConfined, _rootPath, url, _metadata, _headers));

    // assert that we did set the initial state correctly
    // gets for internal state

    QCOMPARE(download->downloadId(), id);
    QCOMPARE(download->path(), path);
    QCOMPARE(download->url(), url);
    QCOMPARE(download->state(), Download::IDLE);
    QCOMPARE(download->progress(), 0ULL);
    QCOMPARE(download->totalSize(), 0ULL);
    verifyMocks();
}

void
TestDownload::testHashConstructor_data() {
    QTest::addColumn<QString>("id");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QString>("hash");
    QTest::addColumn<QString>("algo");

    QTest::newRow("First row") << UuidUtils::getDBusString(QUuid::createUuid())
        << "/path/to/first/app" << QUrl("http://ubuntu.com")
        << "my-first-hash" << "md5";
    QTest::newRow("Second row") << UuidUtils::getDBusString(QUuid::createUuid())
        << "/path/to/second/app" << QUrl("http://ubuntu.com/juju")
        << "my-second-hash" << "Md5";
    QTest::newRow("Third row") << UuidUtils::getDBusString(QUuid::createUuid())
        << "/path/to/third/app" << QUrl("http://ubuntu.com/tablet")
        << "my-third-hash" << "Sha1";
    QTest::newRow("Last row") << UuidUtils::getDBusString(QUuid::createUuid())
        << "/path/to/last/app" << QUrl("http://ubuntu.com/phone")
        << "my-last-hash" << "Sha256";
}

void
TestDownload::testHashConstructor() {
    QFETCH(QString, id);
    QFETCH(QString, path);
    QFETCH(QUrl, url);
    QFETCH(QString, hash);
    QFETCH(QString, algo);

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QScopedPointer<FileDownload> download(new FileDownload(id, path, _isConfined,
        _rootPath, url, hash, algo, _metadata, _headers));

    QCOMPARE(download->downloadId(), id);
    QCOMPARE(download->path(), path);
    QCOMPARE(download->url(), url);
    QCOMPARE(download->hash(), hash);
    QCOMPARE(download->hashAlgorithm(), HashAlgorithm::getHashAlgo(algo));
    QCOMPARE(download->state(), Download::IDLE);
    QCOMPARE(download->progress(), 0ULL);
    QCOMPARE(download->totalSize(), 0ULL);
    verifyMocks();
}

void
TestDownload::testPath_data() {
    // create a number of rows with a diff path to ensure that
    // the accessor does return the correct one
    QTest::addColumn<QString>("path");
    QTest::newRow("First row") << "/first/random/path";
    QTest::newRow("Second row") << "/second/random/path";
    QTest::newRow("Third row") << "/third/random/path";
    QTest::newRow("Last row") << "/last/random/path";
}

void
TestDownload::testPath() {
    // create an app download and assert that the returned data is correct
    QFETCH(QString, path);

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QScopedPointer<FileDownload> download(new FileDownload(_id, path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QCOMPARE(download->path(), path);
    verifyMocks();
}

void
TestDownload::testUrl_data() {
    // create a number of rows with a diff url to ensure that
    // the accessor does return the correct one
    QTest::addColumn<QUrl>("url");
    QTest::newRow("First row") << QUrl("http://ubuntu.com");
    QTest::newRow("Second row") << QUrl("http://one.ubuntu.com");
    QTest::newRow("Third row") << QUrl("http://ubuntu.com/phone");
    QTest::newRow("Last row") << QUrl("http://ubuntu.com/tablet");
}

void
TestDownload::testUrl() {
    // create an app download and assert that the returned data is correct
    QFETCH(QUrl, url);

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, url, _metadata, _headers));
    QCOMPARE(download->url(), url);
    verifyMocks();
}

void
TestDownload::testProgress_data() {
    QTest::addColumn<QByteArray>("fileData");
    QTest::addColumn<qulonglong>("received");
    QTest::addColumn<qint64>("total");

    QTest::newRow("First row") << QByteArray(0, 'f') << qulonglong(67)
        << qint64(200);
    QTest::newRow("Second row") << QByteArray(200, 's') << qulonglong(45)
        << qint64(12000);
    QTest::newRow("Third row") << QByteArray(300, 't') << qulonglong(2)
        << qint64(2345);
    QTest::newRow("Last row") << QByteArray(400, 'l') << qulonglong(3434)
        << qint64(2323);
}

void
TestDownload::testProgress() {
    QFETCH(QByteArray, fileData);
    QFETCH(qulonglong, received);
    QFETCH(qint64, total);
    auto file = new MockFile("test");
    auto reply = new MockNetworkReply();

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    // set expectations to get the request and the reply correctly

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, readAll())
        .Times(1)
        .WillOnce(Return(fileData));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, write(fileData))
        .Times(1)
        .WillOnce(Return(0));

    EXPECT_CALL(*file, flush())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, size())
        .Times(1)
        .WillOnce(Return(fileData.size()));

    EXPECT_CALL(*file, close())
        .Times(1);

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);
    QSignalSpy spy(download,
        SIGNAL(progress(qulonglong, qulonglong)));

    // start the download so that we do have access to the reply
    download->start();  // change state
    download->startDownload();

    reply->downloadProgress(received, qulonglong(total));

    // assert that the total is set and that the signals is emitted
    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(download->totalSize(), qulonglong(total));

    QList<QVariant> arguments = spy.takeFirst();
    // assert that the size is not the received but the file size
    QCOMPARE(arguments.at(0).toULongLong(), (qulonglong)fileData.size());
    QCOMPARE(arguments.at(1).toULongLong(), qulonglong(total));

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file));
    QVERIFY(Mock::VerifyAndClearExpectations(reply));
    verifyMocks();
}

void
TestDownload::testProgressNotKnownSize_data() {
    QTest::addColumn<QByteArray>("fileData");
    QTest::addColumn<qulonglong>("received");
    QTest::addColumn<int>("total");

    QTest::newRow("First row") << QByteArray(0, 'f') << 67ULL << -1;
    QTest::newRow("Second row") << QByteArray(200, 's') << 45ULL << -1;
    QTest::newRow("Third row") << QByteArray(300, 't') << 2ULL << -1;
    QTest::newRow("Last row") << QByteArray(400, 'l') << 3434ULL << -1;
}

void
TestDownload::testProgressNotKnownSize() {
    QFETCH(QByteArray, fileData);
    QFETCH(qulonglong, received);
    QFETCH(int, total);
    auto file = new MockFile("test");
    auto reply = new MockNetworkReply();

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    // set expectations to get the request and the reply correctly

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, readAll())
        .Times(1)
        .WillOnce(Return(fileData));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, write(fileData))
        .Times(1)
        .WillOnce(Return(0));

    EXPECT_CALL(*file, flush())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, size())
        .Times(1)
        .WillOnce(Return(fileData.size()));

    EXPECT_CALL(*file, close())
        .Times(1);

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);
    QSignalSpy spy(download,
        SIGNAL(progress(qulonglong, qulonglong)));

    // start the download so that we do have access to the reply
    download->start();  // change state
    download->startDownload();

    emit reply->downloadProgress(received, total);

    QTRY_COMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    qulonglong size = (qulonglong)fileData.size();
    QCOMPARE(arguments.at(0).toULongLong(), size);
    // must be the same as the progress
    QCOMPARE(arguments.at(1).toULongLong(), size);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file));
    QVERIFY(Mock::VerifyAndClearExpectations(reply));
    verifyMocks();
}

void
TestDownload::testTotalSize() {
    qulonglong received = 30ULL;
    qulonglong total = 200ULL;
    auto file = new MockFile("test");
    auto reply = new MockNetworkReply();

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    // set expectations to get the request and the reply correctly

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, readAll())
        .Times(2)
        .WillOnce(Return(QByteArray()))
        .WillOnce(Return(QByteArray()));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, write(_))
        .Times(2)
        .WillOnce(Return(0))
        .WillOnce(Return(0));

    EXPECT_CALL(*file, flush())
        .Times(2)
        .WillOnce(Return(true))
        .WillOnce(Return(true));

    EXPECT_CALL(*file, size())
        .Times(2)
        .WillOnce(Return(0))
        .WillOnce(Return(0));

    EXPECT_CALL(*file, close())
        .Times(1);

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);
    QSignalSpy spy(download,
        SIGNAL(progress(qulonglong, qulonglong)));

    // start the download so that we do have access to the reply
    download->start();  // change state
    download->startDownload();

    // assert that the total size is just set once
    // by emitting two signals with diff sizes

    emit reply->downloadProgress(received, total);
    emit reply->downloadProgress(received, 2*total);

    QCOMPARE(download->totalSize(), total);
    QCOMPARE(spy.count(), 2);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file));
    QVERIFY(Mock::VerifyAndClearExpectations(reply));
    verifyMocks();
}

void
TestDownload::testTotalSizeNoProgress() {
    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers));
    QCOMPARE(0ULL, download->totalSize());
    verifyMocks();
}

void
TestDownload::testSetThrottleNoReply_data() {
    QTest::addColumn<qulonglong>("speed");

    QTest::newRow("First row") << 200ULL;
    QTest::newRow("Second row") << 1212ULL;
    QTest::newRow("Third row") << 998ULL;
    QTest::newRow("Last row") << 60ULL;
}

void
TestDownload::testSetThrottleNoReply() {
    QFETCH(qulonglong, speed);
    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers));
    download->setThrottle(speed);
    QCOMPARE(speed, download->throttle());
    verifyMocks();
}

void
TestDownload::testSetThrottle_data() {
    QTest::addColumn<uint>("speed");

    QTest::newRow("First row") << 200u;
    QTest::newRow("Second row") << 1212u;
    QTest::newRow("Third row") << 998u;
    QTest::newRow("Last row") << 60u;
}

void
TestDownload::testSetThrottle() {
    QFETCH(uint, speed);

    auto file = new MockFile("test");
    auto reply = new MockNetworkReply();

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    // set expectations to get the request and the reply correctly

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, setReadBufferSize(0))
        .Times(1);

    EXPECT_CALL(*reply, setReadBufferSize(speed))
        .Times(1);

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, close())
        .Times(1);

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);

    download->start();
    download->startDownload();
    download->setThrottle(speed);
    QCOMPARE(speed, download->throttle());

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file));
    QVERIFY(Mock::VerifyAndClearExpectations(reply));
    verifyMocks();
}

void
TestDownload::testSetGSMDownloadSame_data() {
    QTest::addColumn<bool>("value");

    QTest::newRow("True") << true;
    QTest::newRow("False") << false;
}

void
TestDownload::testSetGSMDownloadSame() {
    QFETCH(bool, value);

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers));
    download->allowGSMDownload(value);
    QSignalSpy spy(download.data(), SIGNAL(stateChanged()));

    download->allowGSMDownload(value);
    QCOMPARE(spy.count(), 0);
    verifyMocks();
}

void
TestDownload::testSetGSMDownloadDiff_data() {
    QTest::addColumn<bool>("oldValue");
    QTest::addColumn<bool>("newValue");

    QTest::newRow("True") << true << false;
    QTest::newRow("False") << false << true;
}

void
TestDownload::testSetGSMDownloadDiff() {
    QFETCH(bool, oldValue);
    QFETCH(bool, newValue);

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers));
    download->allowGSMDownload(oldValue);
    QSignalSpy spy(download.data(), SIGNAL(stateChanged()));

    download->allowGSMDownload(newValue);
    QCOMPARE(spy.count(), 1);
    verifyMocks();
}

void
TestDownload::testCanDownloadGSM_data() {
    QTest::addColumn<QVariant>("mode");

    QVariant gsmMode, cdmaMode, wCdmaMode, wlanMode, ethernetMode,
        bluetoothMode, wimaxMode, lteMode, tdscdmaMode;

    gsmMode.setValue(QNetworkInfo::GsmMode);
    cdmaMode.setValue(QNetworkInfo::CdmaMode);
    wCdmaMode.setValue(QNetworkInfo::WcdmaMode);
    wlanMode.setValue(QNetworkInfo::WlanMode);
    ethernetMode.setValue(QNetworkInfo::EthernetMode);
    bluetoothMode.setValue(QNetworkInfo::BluetoothMode);
    wimaxMode.setValue(QNetworkInfo::WimaxMode);
    lteMode.setValue(QNetworkInfo::LteMode);
    tdscdmaMode.setValue(QNetworkInfo::TdscdmaMode);

    QTest::newRow("GSM Mode") << gsmMode;
    QTest::newRow("CDMA Mode") << cdmaMode;
    QTest::newRow("WCDMA Mode") << wCdmaMode;
    QTest::newRow("Wlan Mode") << wlanMode;
    QTest::newRow("Ethernet Mode") << ethernetMode;
    QTest::newRow("Bluetooth Mode") << bluetoothMode;
    QTest::newRow("WIMAX Mode") << wimaxMode;
    QTest::newRow("LTE Mode") << lteMode;
    QTest::newRow("TDSCDMA Mode") << tdscdmaMode;
}

void
TestDownload::testCanDownloadGSM() {
    QFETCH(QVariant, mode);

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_networkInfo, currentNetworkMode())
        .Times(1)
        .WillOnce(Return(mode.value<QNetworkInfo::NetworkMode>()));

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers));
    download->allowGSMDownload(true);
    QVERIFY(download->canDownload());
    verifyMocks();
}

void
TestDownload::testCanDownloadNoGSM_data() {
    QTest::addColumn<QVariant>("mode");
    QTest::addColumn<bool>("result");

    QVariant unknownMode, gsmMode, cdmaMode, wCdmaMode, wlanMode, ethernetMode,
        bluetoothMode, wimaxMode, lteMode, tdscdmaMode;

    unknownMode.setValue(QNetworkInfo::UnknownMode);
    gsmMode.setValue(QNetworkInfo::GsmMode);
    cdmaMode.setValue(QNetworkInfo::CdmaMode);
    wCdmaMode.setValue(QNetworkInfo::WcdmaMode);
    wlanMode.setValue(QNetworkInfo::WlanMode);
    ethernetMode.setValue(QNetworkInfo::EthernetMode);
    bluetoothMode.setValue(QNetworkInfo::BluetoothMode);
    wimaxMode.setValue(QNetworkInfo::WimaxMode);
    lteMode.setValue(QNetworkInfo::LteMode);
    tdscdmaMode.setValue(QNetworkInfo::TdscdmaMode);

    QTest::newRow("Unknown Mode") << unknownMode << false;
    QTest::newRow("GSM Mode") << gsmMode << false;
    QTest::newRow("CDMA Mode") << cdmaMode << false;
    QTest::newRow("WCDMA Mode") << wCdmaMode << false;
    QTest::newRow("Wlan Mode") << wlanMode << true;
    QTest::newRow("Ethernet Mode") << ethernetMode << true;
    QTest::newRow("Bluetooth Mode") << bluetoothMode << true;
    QTest::newRow("WIMAX Mode") << wimaxMode << false;
    QTest::newRow("LTE Mode") << lteMode << false;
    QTest::newRow("TDSCDMA Mode") << tdscdmaMode << false;
}

void
TestDownload::testCanDownloadNoGSM() {
    QFETCH(QVariant, mode);
    QFETCH(bool, result);

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_networkInfo, currentNetworkMode())
        .Times(1)
        .WillOnce(Return(mode.value<QNetworkInfo::NetworkMode>()));

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers));
    download->allowGSMDownload(false);

    QCOMPARE(result, download->canDownload());
    verifyMocks();
}

void
TestDownload::testCancel() {
    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(stateChanged()));
    download->cancel();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::CANCEL);
    verifyMocks();
}

void
TestDownload::testPause() {
    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(stateChanged()));
    download->pause();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::PAUSE);
    verifyMocks();
}

void
TestDownload::testResume() {
    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(stateChanged()));
    download->resume();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::RESUME);
    verifyMocks();
}

void
TestDownload::testStart() {
    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(stateChanged()));
    download->start();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::START);
    verifyMocks();
}

void
TestDownload::testCancelDownload() {
    QScopedPointer<MockFile> file(new MockFile("test"));
    QScopedPointer<MockNetworkReply> reply(new MockNetworkReply());

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply.data()));

    EXPECT_CALL(*reply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply.data(), abort())
        .Times(1);

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file.data()));

    EXPECT_CALL(*file.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file.data(), remove())
        .Times(1)
        .WillOnce(Return(true));

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(),
        SIGNAL(canceled(bool)));  // NOLINT(readability/function)

    download->start();  // change state
    download->startDownload();
    download->cancel();  // change state
    download->cancelDownload();  // method under test

    // assert that method was indeed called
    QVERIFY(Mock::VerifyAndClearExpectations(file.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(reply.data()));
    verifyMocks();
}

void
TestDownload::testCancelDownloadNotStarted() {
    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(0);

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(),
        SIGNAL(canceled(bool)));  // NOLINT(readability/function)

    download->cancel();  // change state
    download->cancelDownload();  // method under test

    // assert that method was indeed called
    verifyMocks();
}

void
TestDownload::testPauseDownload() {
    QByteArray fileData(0, 'f');
    auto file = new MockFile("test");
    QScopedPointer<MockNetworkReply> reply(new MockNetworkReply());

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply.data()));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply, readAll())
        .Times(1)
        .WillOnce(Return(fileData));

    EXPECT_CALL(*reply, abort())
        .Times(1);

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, write(fileData))
        .Times(1)
        .WillOnce(Return(0));

    EXPECT_CALL(*file, flush())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, close())
        .Times(1);

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);
    QSignalSpy spy(download,
        SIGNAL(paused(bool)));  // NOLINT(readability/function)

    download->start();  // change state
    download->startDownload();
    download->pause();  // change state
    download->pauseDownload();  // method under test

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toBool());

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file));
    QVERIFY(Mock::VerifyAndClearExpectations(reply.data()));
    verifyMocks();
}

void
TestDownload::testPauseDownloadNotStarted() {
    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(),
        SIGNAL(paused(bool)));  // NOLINT(readability/function)

    download->pause();
    download->pauseDownload();

    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(!arguments.at(0).toBool());
    verifyMocks();
}

void
TestDownload::testResumeRunning() {
    auto file = new MockFile("test");
    auto reply = new MockNetworkReply();

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, close())
        .Times(1);

    auto download = new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers);
    QSignalSpy spy(download,
        SIGNAL(resumed(bool)));  // NOLINT(readability/function)

    download->start();
    download->startDownload();
    download->resume();
    download->resumeDownload();

    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(!arguments.at(0).toBool());

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file));
    QVERIFY(Mock::VerifyAndClearExpectations(reply));
    verifyMocks();
}

void
TestDownload::testResumeDownload() {
    QByteArray fileData(0, 'f');
    auto file = new MockFile("test");
    auto firstReply = new MockNetworkReply();
    auto secondReply = new MockNetworkReply();

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(2)
        .WillOnce(Return(firstReply))
        .WillOnce(Return(secondReply));

    EXPECT_CALL(*firstReply, setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*secondReply, setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*firstReply, readAll())
        .Times(1)
        .WillOnce(Return(fileData));

    EXPECT_CALL(*secondReply, readAll())
        .Times(0);

    EXPECT_CALL(*firstReply, abort())
        .Times(1);

    EXPECT_CALL(*secondReply, abort())
        .Times(0);

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, write(fileData))
        .Times(1)
        .WillOnce(Return(fileData.size()));

    EXPECT_CALL(*file, flush())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, close())
        .Times(1);

    EXPECT_CALL(*file, size())
        .Times(1)
        .WillOnce(Return(fileData.size()));

    EXPECT_CALL(*file, remove())
        .Times(0);

    auto download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers));
    QSignalSpy pausedSpy(download, SIGNAL(paused(bool)));
    QSignalSpy resumedSpy(download, SIGNAL(resumed(bool)));

    download->start();  // change state
    download->startDownload();
    download->pause();
    download->pauseDownload();
    download->resume();
    download->resumeDownload();

    QCOMPARE(1, pausedSpy.count());
    auto arguments = pausedSpy.takeFirst();
    QVERIFY(arguments.at(0).toBool());

    QCOMPARE(1, resumedSpy.count());
    arguments = resumedSpy.takeFirst();
    QVERIFY(arguments.at(0).toBool());

    delete firstReply;
    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file));
    QVERIFY(Mock::VerifyAndClearExpectations(firstReply));
    QVERIFY(Mock::VerifyAndClearExpectations(secondReply));
    verifyMocks();
}

void
TestDownload::testStartDownload() {
    auto file = new MockFile("test");
    auto reply = new MockNetworkReply();

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, remove())
        .Times(0);

    EXPECT_CALL(*file, close())
        .Times(1);

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);
    QSignalSpy spy(download,
        SIGNAL(started(bool)));  // NOLINT(readability/function)

    download->start();  // change state
    download->startDownload();

    QCOMPARE(1, spy.count());
    auto arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toBool());

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file));
    QVERIFY(Mock::VerifyAndClearExpectations(reply));
    verifyMocks();
}

void
TestDownload::testStartDownloadAlreadyStarted() {
    auto file = new MockFile("test");
    auto reply = new MockNetworkReply();

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, remove())
        .Times(0);

    EXPECT_CALL(*file, close())
        .Times(1);

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);
    QSignalSpy spy(download,
        SIGNAL(started(bool)));  // NOLINT(readability/function)

    download->start();  // change state
    download->startDownload();
    download->startDownload();

    QCOMPARE(2, spy.count());
    auto arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toBool());
    arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toBool());

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file));
    QVERIFY(Mock::VerifyAndClearExpectations(reply));
    verifyMocks();
}

void
TestDownload::testOnSuccessNoHash() {
    auto file = new MockFile("test");
    QScopedPointer<MockNetworkReply> reply(new MockNetworkReply());

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply.data()));

    EXPECT_CALL(*reply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply.data(), attribute(_))
        .Times(1)
        .WillOnce(Return(QVariant(200)));

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, remove())
        .Times(0);

    EXPECT_CALL(*file, close())
        .Times(1);

    EXPECT_CALL(*_cryptoFactory, createCryptographicHash(_, _))
        .Times(0);

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);
    QSignalSpy spy(download, SIGNAL(finished(QString)));
    QSignalSpy processingSpy(download, SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    // emit the finish signal and expect it to be raised
    emit reply->finished();
    QTRY_COMPARE(spy.count(), 1);
    QTRY_COMPARE(processingSpy.count(), 0);
    QCOMPARE(download->state(), Download::FINISH);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file));
    QVERIFY(Mock::VerifyAndClearExpectations(reply.data()));
    verifyMocks();
}

void
TestDownload::testOnSuccessHashError() {
    QScopedPointer<MockFile> file(new MockFile("test"));
    QScopedPointer<MockNetworkReply> reply(new MockNetworkReply());
    auto hash = new MockCryptographicHash();

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply.data()));

    EXPECT_CALL(*reply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply.data(), attribute(_))
        .Times(1)
        .WillOnce(Return(QVariant(200)));

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file.data()));

    EXPECT_CALL(*file.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file.data(), reset())
        .Times(1);

    EXPECT_CALL(*file.data(), remove())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file.data(), device())
        .Times(1)
        .WillOnce(Return(nullptr));


    EXPECT_CALL(*_cryptoFactory, createCryptographicHash(_, _))
        .Times(1)
        .WillOnce(Return(hash));

    EXPECT_CALL(*hash, addData(_))
        .Times(1);

    EXPECT_CALL(*hash, result())
        .Times(1)
        .WillOnce(Return(QByteArray()));

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, "imposible-hash-is-not-hex",
        _algo, _metadata, _headers);

    QSignalSpy errorSpy(download, SIGNAL(error(QString)));
    QSignalSpy processingSpy(download, SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    emit reply->finished();

    // the has is a random string so we should get an error signal
    QTRY_COMPARE(errorSpy.count(), 1);
    QTRY_COMPARE(processingSpy.count(), 1);
    QCOMPARE(download->state(), Download::ERROR);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(hash));
    QVERIFY(Mock::VerifyAndClearExpectations(file.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(reply.data()));
    verifyMocks();
}

void
TestDownload::testOnSuccessHash() {
    auto file = new MockFile("test");
    QScopedPointer<MockNetworkReply> reply(new MockNetworkReply());
    QByteArray hashData(100, 'f');
    auto hashString = QString(hashData.toHex());
    auto hash = new MockCryptographicHash();

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply.data()));

    EXPECT_CALL(*reply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply.data(), attribute(_))
        .Times(1)
        .WillOnce(Return(QVariant(200)));

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, reset())
        .Times(1);

    EXPECT_CALL(*file, remove())
        .Times(0);

    EXPECT_CALL(*file, close())
        .Times(1);

    EXPECT_CALL(*file, device())
        .Times(1)
        .WillOnce(Return(nullptr));

    EXPECT_CALL(*_cryptoFactory, createCryptographicHash(_, _))
        .Times(1)
        .WillOnce(Return(hash));

    EXPECT_CALL(*hash, addData(_))
        .Times(1);

    EXPECT_CALL(*hash, result())
        .Times(1)
        .WillOnce(Return(hashData));

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, hashString, _algo, _metadata,
        _headers);
    QSignalSpy spy(download, SIGNAL(finished(QString)));
    QSignalSpy processingSpy(download, SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    emit reply->finished();

    // the hash should be correct and we should get the finish signal
    QTRY_COMPARE(spy.count(), 1);
    QTRY_COMPARE(processingSpy.count(), 1);
    QCOMPARE(download->state(), Download::FINISH);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(hash));
    QVERIFY(Mock::VerifyAndClearExpectations(file));
    QVERIFY(Mock::VerifyAndClearExpectations(reply.data()));
    verifyMocks();
}

void
TestDownload::testOnHttpError_data() {
    QTest::addColumn<int>("code");
    QTest::addColumn<QString>("message");

    QTest::newRow("Not Found") << 404 << "Not Found";
    QTest::newRow("Method Not Allowed") << 405 << "Method Not Allowed";
    QTest::newRow("Not Acceptable") << 406 << "Not Acceptable";
    QTest::newRow("Request Timeout") << 408 << "Request Timeout";
}

void
TestDownload::testOnHttpError() {
    QFETCH(int, code);
    QFETCH(QString, message);
    QScopedPointer<MockFile> file(new MockFile("test"));
    QScopedPointer<MockNetworkReply> reply(new MockNetworkReply());

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply.data()));

    EXPECT_CALL(*reply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply.data(),
             attribute(QNetworkRequest::HttpStatusCodeAttribute))
        .Times(1)
        .WillOnce(Return(QVariant(code)));

    EXPECT_CALL(*reply.data(),
             attribute(QNetworkRequest::HttpReasonPhraseAttribute))
        .Times(1)
        .WillOnce(Return(QVariant(message)));

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file.data()));

    EXPECT_CALL(*file.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file.data(), remove())
        .Times(1)
        .WillOnce(Return(true));

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);
    QSignalSpy errorSpy(download, SIGNAL(error(QString)));
    QSignalSpy httpErrorSpy(download, SIGNAL(httpError(HttpErrorStruct)));

    download->start();  // change state
    download->startDownload();

    // emit the error and ensure that the signals are raised
    reply->error(QNetworkReply::ContentAccessDenied);
    QCOMPARE(httpErrorSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 1);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(reply.data()));
    verifyMocks();
}

void
TestDownload::testOnSslError() {
    QScopedPointer<MockFile> file(new MockFile("test"));
    QScopedPointer<MockNetworkReply> reply(new MockNetworkReply());

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply.data()));

    EXPECT_CALL(*reply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply.data(), canIgnoreSslErrors(_))
        .Times(1)
        .WillOnce(Return(false));

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file.data()));

    EXPECT_CALL(*file.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file.data(), remove())
        .Times(1)
        .WillOnce(Return(true));
    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);

    QSignalSpy spy(download, SIGNAL(error(QString)));

    download->start();  // change state
    download->startDownload();

    QList<QSslError> errors;
    emit reply->sslErrors(errors);
    QTRY_COMPARE(spy.count(), 1);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(reply.data()));
    verifyMocks();
}

void
TestDownload::testOnNetworkError_data() {
    QTest::addColumn<int>("code");

    QTest::newRow("Connection Refused Error") << 1;
    QTest::newRow("RemoteHost Closed Error") << 2;
    QTest::newRow("Host Not Found Error") << 3;
    QTest::newRow("Timeout Error") << 4;
    QTest::newRow("Operation Canceled Error") << 4;
}

void
TestDownload::testOnNetworkError() {
    QFETCH(int, code);
    QScopedPointer<MockFile> file(new MockFile("test"));
    QScopedPointer<MockNetworkReply> reply(new MockNetworkReply());

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply.data()));

    EXPECT_CALL(*reply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply.data(), attribute(_))
        .Times(1)
        .WillOnce(Return(QVariant()));  // invalid variant

    EXPECT_CALL(*reply.data(), errorString())
        .Times(1)
        .WillOnce(Return(QString("error")));  // invalid variant


    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file.data()));

    EXPECT_CALL(*file.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file.data(), remove())
        .Times(1)
        .WillOnce(Return(true));

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);
    QSignalSpy errorSpy(download, SIGNAL(error(QString)));
    QSignalSpy networkErrorSpy(download,
        SIGNAL(networkError(NetworkErrorStruct)));

    download->start();  // change state
    download->startDownload();

    // emit the error and ensure that the signals are raised
    reply->error((QNetworkReply::NetworkError)code);
    QCOMPARE(networkErrorSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 1);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(reply.data()));
    verifyMocks();
}

void
TestDownload::testOnAuthError() {
    QScopedPointer<MockFile> file(new MockFile("test"));
    QScopedPointer<MockNetworkReply> reply(new MockNetworkReply());

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply.data()));

    EXPECT_CALL(*reply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply.data(), attribute(_))
        .Times(1)
        .WillOnce(Return(QVariant()));  // invalid variant

    EXPECT_CALL(*reply.data(), errorString())
        .Times(1)
        .WillOnce(Return(QString("error")));  // invalid variant


    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file.data()));

    EXPECT_CALL(*file.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file.data(), remove())
        .Times(1)
        .WillOnce(Return(true));
    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);

    QSignalSpy errorSpy(download, SIGNAL(error(QString)));
    QSignalSpy authErrorSpy(download,
        SIGNAL(authError(AuthErrorStruct)));
    QSignalSpy networkErrorSpy(download,
        SIGNAL(networkError(NetworkErrorStruct)));

    download->start();  // change state
    download->startDownload();

    // emit the error and ensure that the signals are raised
    reply->error(QNetworkReply::AuthenticationRequiredError);

    QCOMPARE(networkErrorSpy.count(), 0);
    QCOMPARE(authErrorSpy.count(), 1);
    auto error = authErrorSpy.takeFirst().at(0).value<AuthErrorStruct>();
    QVERIFY(error.getType() == AuthErrorStruct::Server);
    QCOMPARE(errorSpy.count(), 1);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(reply.data()));
    verifyMocks();
}

void
TestDownload::testOnProxyAuthError() {
    QScopedPointer<MockFile> file(new MockFile("test"));
    QScopedPointer<MockNetworkReply> reply(new MockNetworkReply());

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply.data()));

    EXPECT_CALL(*reply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply.data(), attribute(_))
        .Times(1)
        .WillOnce(Return(QVariant()));  // invalid variant

    EXPECT_CALL(*reply.data(), errorString())
        .Times(1)
        .WillOnce(Return(QString("error")));  // invalid variant


    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file.data()));

    EXPECT_CALL(*file.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file.data(), remove())
        .Times(1)
        .WillOnce(Return(true));

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);
    QSignalSpy errorSpy(download, SIGNAL(error(QString)));
    QSignalSpy authErrorSpy(download,
        SIGNAL(authError(AuthErrorStruct)));
    QSignalSpy networkErrorSpy(download,
        SIGNAL(networkError(NetworkErrorStruct)));

    download->start();  // change state
    download->startDownload();

    // emit the error and ensure that the signals are raised
    reply->error(QNetworkReply::ProxyAuthenticationRequiredError);
    QCOMPARE(networkErrorSpy.count(), 0);
    QCOMPARE(authErrorSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 1);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(reply.data()));
    verifyMocks();
}

void
TestDownload::testSetRawHeadersStart_data() {
    QTest::addColumn<QMap<QString, QString> >("headers");

    // create a number of headers to assert that thy are added in the request
    StringMap first, second, third;

    // add headers to be added except range
    first["Accept"] = "text/plain";
    first["Accept-Charset"] = "utf-8";

    QTest::newRow("First row") << first;

    second["Accept-Language"] = "en-US";
    second["Cache-Control"] = "no-cache";
    second["Connection"] = "keep-alive";

    QTest::newRow("Second row") << second;

    third["Content-Length"] = "348";
    third["User-Agent"] = "Mozilla/5.0";

    QTest::newRow("Third row") << third;
}

void
TestDownload::testSetRawHeadersStart() {
    QFETCH(StringMap, headers);
    auto file = new MockFile("test");
    auto reply = new MockNetworkReply();

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(RequestHeadersEq(headers)))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, remove())
        .Times(0);

    EXPECT_CALL(*file, close())
        .Times(1);

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, headers);

    download->start();  // change state
    download->startDownload();

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file));
    QVERIFY(Mock::VerifyAndClearExpectations(reply));
    verifyMocks();
}

void
TestDownload::testSetRawHeadersWithRangeStart_data() {
    QTest::addColumn<StringMap>("headers");

    // create a number of headers to assert that thy are added in the request
    StringMap first, second, third;

    // add headers to be added except range
    first["Accept"] = "text/plain";
    first["Accept-Charset"] = "utf-8";
    first["Range"] = "gzip, deflate";

    QTest::newRow("First row") << first;

    second["Accept-Language"] = "en-US";
    second["Cache-Control"] = "no-cache";
    second["Connection"] = "keep-alive";
    second["Range"] = "gzip, deflate";

    QTest::newRow("Second row") << second;

    third["Content-Length"] = "348";
    third["User-Agent"] = "Mozilla/5.0";
    third["Range"] = "gzip, deflate";

    QTest::newRow("Third row") << third;
}

void
TestDownload::testSetRawHeadersWithRangeStart() {
    QFETCH(StringMap, headers);
    auto file = new MockFile("test");
    auto reply = new MockNetworkReply();

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory,
            get(RequestDoesNotHaveHeader(QString("Range"))))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, remove())
        .Times(0);

    EXPECT_CALL(*file, close())
        .Times(1);

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, headers);

    download->start();  // change state
    download->startDownload();

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file));
    QVERIFY(Mock::VerifyAndClearExpectations(reply));
    verifyMocks();
}

void
TestDownload::testSetRawHeadersResume_data() {
    // same as with start but we test that all the headers
    // are added + the range one
    QTest::addColumn<StringMap>("headers");

    // create a number of headers to assert that thy are added in the request
    StringMap first, second, third;

    // add headers to be added except range
    first["Accept"] = "text/plain";
    first["Accept-Charset"] = "utf-8";

    QTest::newRow("First row") << first;

    second["Accept-Language"] = "en-US";
    second["Cache-Control"] = "no-cache";
    second["Connection"] = "keep-alive";

    QTest::newRow("Second row") << second;

    third["Content-Length"] = "348";
    third["User-Agent"] = "Mozilla/5.0";

    QTest::newRow("Third row") << third;
}

void
TestDownload::testSetRawHeadersResume() {
    QFETCH(StringMap, headers);
    QByteArray data;  // empty data to ensure we do not send the range header
    auto file = new MockFile("test");
    QScopedPointer<MockNetworkReply> firstReply(new MockNetworkReply());
    auto secondReply= new MockNetworkReply();
    qint64 size = 300;

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QPair<QString, QString> rangeHeader("Range", QString::number(size));
    EXPECT_CALL(*_reqFactory,
            get(AnyOf(RequestHeadersEq(headers),
                      RequestHasHeader(rangeHeader))))
        .Times(2)
        .WillOnce(Return(firstReply.data()))
        .WillOnce(Return(secondReply));

    EXPECT_CALL(*firstReply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*secondReply, setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*firstReply.data(), abort())
        .Times(1);

    EXPECT_CALL(*firstReply.data(), readAll())
        .Times(1)
        .WillOnce(Return(data));

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, remove())
        .Times(0);

    EXPECT_CALL(*file, write(data))
        .Times(1)
        .WillOnce(Return(data.size()));

    EXPECT_CALL(*file, flush())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, size())
        .Times(1)
        .WillOnce(Return(size));

    EXPECT_CALL(*file, close())
        .Times(1);

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, headers);

    download->start();  // change state
    download->startDownload();
    download->pause();
    download->pauseDownload();
    download->resume();
    download->resumeDownload();

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file));
    QVERIFY(Mock::VerifyAndClearExpectations(firstReply.data()));
    verifyMocks();
}

void
TestDownload::testProcessExecutedNoParams_data() {
    QTest::addColumn<QString>("command");
    QTest::addColumn<QVariantMap>("metadata");
    QVariantMap first, second, third;
    QStringList firstCommand, secondCommand, thirdCommand;

    firstCommand << "touch";
    first["post-download-command"] = firstCommand;

    QTest::newRow("First row") << firstCommand[0] << first;

    secondCommand << "sudo";
    second["post-download-command"] = secondCommand;

    QTest::newRow("Second row") << secondCommand[0] << second;

    thirdCommand << "grep";
    third["post-download-command"] = thirdCommand;

    QTest::newRow("Third row") << thirdCommand[0] << third;
}

void
TestDownload::testProcessExecutedNoParams() {
    QFETCH(QString, command);
    QFETCH(QVariantMap, metadata);
    QStringList args;  // not args

    QScopedPointer<MockFile> file(new MockFile("test"));
    auto reply = new MockNetworkReply();
    QScopedPointer<MockProcess> process(new MockProcess());

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply, attribute(_))
        .Times(1)
        .WillOnce(Return(QVariant(200)));

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file.data()));

    EXPECT_CALL(*file.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file.data(), remove())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_cryptoFactory, createCryptographicHash(_, _))
        .Times(0);

    // process factory and process expectation
    EXPECT_CALL(*_processFactory, createProcess())
        .Times(1)
        .WillOnce(Return(process.data()));

    EXPECT_CALL(*process.data(), start(command, StringListEq(args), _))
        .Times(1);

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, metadata, _headers);

    QSignalSpy spy(download, SIGNAL(finished(QString)));
    QSignalSpy processingSpy(download, SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    // emit the finish signal and expect it to be raised
    emit reply->finished();
    emit process->finished(0, QProcess::NormalExit);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(processingSpy.count(), 1);
    QCOMPARE(download->state(), Download::FINISH);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(reply));
    QVERIFY(Mock::VerifyAndClearExpectations(process.data()));
    verifyMocks();
}

void
TestDownload::testProcessExecutedWithParams_data() {
    QTest::addColumn<QString>("command");
    QTest::addColumn<QStringList>("args");
    QTest::addColumn<QVariantMap>("metadata");
    QVariantMap first, second, third;
    QString firstCommand, secondCommand, thirdCommand;
    QStringList firstArgs, secondArgs, thirdArgs;

    firstArgs << "test-file";
    firstCommand = "touch";
    first["post-download-command"] = QStringList(firstCommand) + firstArgs;

    QTest::newRow("First row") << firstCommand << firstArgs << first;

    secondArgs << "apt-get" << "install" << "click";
    secondCommand = "sudo";
    second["post-download-command"] = QStringList(secondCommand) + secondArgs;

    QTest::newRow("Second row") << secondCommand << secondArgs << second;

    thirdArgs << "." << "-Rn";
    thirdCommand = "grep";
    third["post-download-command"] = QStringList(thirdCommand) + thirdArgs;

    QTest::newRow("Third row") << thirdCommand << thirdArgs << third;
}

void
TestDownload::testProcessExecutedWithParams() {
    QFETCH(QString, command);
    QFETCH(QStringList, args);
    QFETCH(QVariantMap, metadata);

    QScopedPointer<MockFile> file(new MockFile("test"));
    auto reply = new MockNetworkReply();
    QScopedPointer<MockProcess> process(new MockProcess());

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply, attribute(_))
        .Times(1)
        .WillOnce(Return(QVariant(200)));

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file.data()));

    EXPECT_CALL(*file.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file.data(), remove())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_cryptoFactory, createCryptographicHash(_, _))
        .Times(0);

    // process factory and process expectation
    EXPECT_CALL(*_processFactory, createProcess())
        .Times(1)
        .WillOnce(Return(process.data()));

    EXPECT_CALL(*process.data(), start(command, StringListEq(args), _))
        .Times(1);

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, metadata, _headers);

    QSignalSpy spy(download, SIGNAL(finished(QString)));
    QSignalSpy processingSpy(download, SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    // emit the finish signal and expect it to be raised
    emit reply->finished();
    emit process->finished(0, QProcess::NormalExit);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(processingSpy.count(), 1);
    QCOMPARE(download->state(), Download::FINISH);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(reply));
    QVERIFY(Mock::VerifyAndClearExpectations(process.data()));
    verifyMocks();
}

void
TestDownload::testProcessExecutedWithParamsFile_data() {
    QTest::addColumn<QString>("command");
    QTest::addColumn<QStringList>("args");
    QTest::addColumn<QVariantMap>("metadata");
    QVariantMap first, second, third;
    QStringList firstArgs, secondArgs, thirdArgs;
    QString firstCommand, secondCommand, thirdCommand;

    firstArgs << "$file";
    firstCommand = "touch";
    first["post-download-command"] = QStringList(firstCommand) + firstArgs;

    QTest::newRow("First row") << firstCommand << firstArgs << first;

    secondArgs << "apt-get" << "install" << "$file";
    secondCommand = "sudo";
    second["post-download-command"] = QStringList(secondCommand) + secondArgs;

    QTest::newRow("Second row") << secondCommand << secondArgs << second;

    thirdArgs << "$file" << "-Rn";
    thirdCommand = "grep";
    third["post-download-command"] = QStringList(thirdCommand) + thirdArgs;

    QTest::newRow("Third row") << thirdCommand << thirdArgs << third;
}

void
TestDownload::testProcessExecutedWithParamsFile() {
    QFETCH(QString, command);
    QFETCH(QStringList, args);
    QFETCH(QVariantMap, metadata);

    QScopedPointer<MockFile> file(new MockFile("test"));
    auto reply = new MockNetworkReply();
    QScopedPointer<MockProcess> process(new MockProcess());

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply, attribute(_))
        .Times(1)
        .WillOnce(Return(QVariant(200)));

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file.data()));

    EXPECT_CALL(*file.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file.data(), remove())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_cryptoFactory, createCryptographicHash(_, _))
        .Times(0);

    // process factory and process expectation
    EXPECT_CALL(*_processFactory, createProcess())
        .Times(1)
        .WillOnce(Return(process.data()));


    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, metadata, _headers);

    // set the expectation after we know the file path
    QStringList fixedArgs;
    foreach(auto arg, args) {
        if (arg == "$file") {
            fixedArgs << download->filePath();
        } else {
            fixedArgs << arg;
        }
    }
    EXPECT_CALL(*process.data(), start(command, StringListEq(fixedArgs), _))
        .Times(1);

    QSignalSpy spy(download, SIGNAL(finished(QString)));
    QSignalSpy processingSpy(download, SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    // emit the finish signal and expect it to be raised
    emit reply->finished();
    emit process->finished(0, QProcess::NormalExit);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(processingSpy.count(), 1);
    QCOMPARE(download->state(), Download::FINISH);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(reply));
    QVERIFY(Mock::VerifyAndClearExpectations(process.data()));
    verifyMocks();
}

void
TestDownload::testProcessFinishedWithError() {
    auto command = QString("ls");
    QVariantMap metadata;
    metadata["post-download-command"] = QStringList(command);

    QStringList args;  // not args

    QScopedPointer<MockFile> file(new MockFile("test"));
    QScopedPointer<MockNetworkReply> reply(new MockNetworkReply());
    QScopedPointer<MockProcess> process(new MockProcess());

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply.data()));

    EXPECT_CALL(*reply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply.data(), attribute(_))
        .Times(1)
        .WillOnce(Return(QVariant(200)));

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file.data()));

    EXPECT_CALL(*file.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file.data(), remove())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_cryptoFactory, createCryptographicHash(_, _))
        .Times(0);

    // process factory and process expectation
    EXPECT_CALL(*_processFactory, createProcess())
        .Times(1)
        .WillOnce(Return(process.data()));

    EXPECT_CALL(*process.data(), start(command, StringListEq(args), _))
        .Times(1);

    EXPECT_CALL(*process.data(), readAllStandardOutput())
        .Times(1)
        .WillOnce(Return(QByteArray()));

    EXPECT_CALL(*process.data(), readAllStandardError())
        .Times(1)
        .WillOnce(Return(QByteArray()));

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, metadata, _headers);

    QSignalSpy spy(download, SIGNAL(error(QString)));
    QSignalSpy processingSpy(download, SIGNAL(processing(QString)));
    QSignalSpy processErrorSpy(download,
        SIGNAL(processError(ProcessErrorStruct)));

    download->start();  // change state
    download->startDownload();

    // emit the finish signal and expect it to be raised
    emit reply->finished();
    emit process->finished(-1, QProcess::NormalExit);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(processingSpy.count(), 1);
    QCOMPARE(processErrorSpy.count(), 1);
    QCOMPARE(download->state(), Download::ERROR);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(reply.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(process.data()));
    verifyMocks();
}

void
TestDownload::testProcessError_data() {
    QTest::addColumn<int>("code");
    QTest::addColumn<QString>("stdOut");
    QTest::addColumn<QString>("stdErr");

    QTest::newRow("First row") << 1 << "Things hapened" <<
        "Errorooror";
    QTest::newRow("Second row") << 2 << "Time error" <<
        "Oh lords!";
    QTest::newRow("Second row") << 3 << "Read" <<
        "Error!";
}

void
TestDownload::testProcessError() {
    QFETCH(int, code);
    QFETCH(QString, stdOut);
    QFETCH(QString, stdErr);

    auto command = QString("ls");
    QVariantMap metadata;
    metadata["post-download-command"] = QStringList(command);

    QStringList args;  // not args

    QScopedPointer<MockFile> file(new MockFile("test"));
    QScopedPointer<MockNetworkReply> reply(new MockNetworkReply());
    QScopedPointer<MockProcess> process(new MockProcess());

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply.data()));

    EXPECT_CALL(*reply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply.data(), attribute(_))
        .Times(1)
        .WillOnce(Return(QVariant(200)));

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file.data()));

    EXPECT_CALL(*file.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file.data(), remove())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_cryptoFactory, createCryptographicHash(_, _))
        .Times(0);

    // process factory and process expectation
    EXPECT_CALL(*_processFactory, createProcess())
        .Times(1)
        .WillOnce(Return(process.data()));

    EXPECT_CALL(*process.data(), start(command, StringListEq(args), _))
        .Times(1);

    EXPECT_CALL(*process.data(), readAllStandardOutput())
        .Times(1)
        .WillOnce(Return(stdOut.toUtf8()));

    EXPECT_CALL(*process.data(), readAllStandardError())
        .Times(1)
        .WillOnce(Return(stdErr.toUtf8()));

    EXPECT_CALL(*process.data(), program())
        .Times(1)
        .WillOnce(Return(command));

    EXPECT_CALL(*process.data(), arguments())
        .Times(1)
        .WillOnce(Return(QStringList()));

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, metadata, _headers);

    QSignalSpy spy(download, SIGNAL(error(QString)));
    QSignalSpy processingSpy(download, SIGNAL(processing(QString)));
    QSignalSpy processErrorSpy(download,
        SIGNAL(processError(ProcessErrorStruct)));

    download->start();  // change state
    download->startDownload();

    // emit the finish signal and expect it to be raised
    emit reply->finished();
    emit process->error(static_cast<QProcess::ProcessError>(code));

    QCOMPARE(spy.count(), 1);
    QCOMPARE(processingSpy.count(), 1);
    QCOMPARE(processErrorSpy.count(), 1);
    QCOMPARE(download->state(), Download::ERROR);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(reply.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(process.data()));
    verifyMocks();
}

void
TestDownload::testProcessFinishedCrash() {
    auto command = QString("ls");
    QVariantMap metadata;
    metadata["post-download-command"] = QStringList(command);

    QStringList args;  // not args

    QScopedPointer<MockFile> file(new MockFile("test"));
    QScopedPointer<MockNetworkReply> reply(new MockNetworkReply());
    QScopedPointer<MockProcess> process(new MockProcess());

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply.data()));

    EXPECT_CALL(*reply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply.data(), attribute(_))
        .Times(1)
        .WillOnce(Return(QVariant(200)));

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file.data()));

    EXPECT_CALL(*file.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file.data(), remove())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_cryptoFactory, createCryptographicHash(_, _))
        .Times(0);

    // process factory and process expectation
    EXPECT_CALL(*_processFactory, createProcess())
        .Times(1)
        .WillOnce(Return(process.data()));

    EXPECT_CALL(*process.data(), start(command, StringListEq(args), _))
        .Times(1);

    EXPECT_CALL(*process.data(), readAllStandardOutput())
        .Times(1)
        .WillOnce(Return(QByteArray()));

    EXPECT_CALL(*process.data(), readAllStandardError())
        .Times(1)
        .WillOnce(Return(QByteArray()));

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, metadata, _headers);

    QSignalSpy spy(download, SIGNAL(error(QString)));
    QSignalSpy processingSpy(download, SIGNAL(processing(QString)));
    QSignalSpy processErrorSpy(download,
        SIGNAL(processError(ProcessErrorStruct)));

    download->start();  // change state
    download->startDownload();

    // emit the finish signal and expect it to be raised
    emit reply->finished();
    emit process->finished(0, QProcess::CrashExit);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(processingSpy.count(), 1);
    QCOMPARE(processErrorSpy.count(), 1);
    QCOMPARE(download->state(), Download::ERROR);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(reply.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(process.data()));
    verifyMocks();
}

void
TestDownload::testSetRawHeaderAcceptEncoding() {
    auto file = new MockFile("test");
    auto reply = new MockNetworkReply();

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QPair<QString, QString> encodingHeader("Accept-Encoding",
        QString("identity"));

    EXPECT_CALL(*_reqFactory, get(RequestHasHeader(encodingHeader)))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, remove())
        .Times(0);

    EXPECT_CALL(*file, close())
        .Times(1);

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);
    QSignalSpy spy(download,
        SIGNAL(started(bool)));  // NOLINT(readability/function)

    download->start();  // change state
    download->startDownload();

    QCOMPARE(1, spy.count());
    auto arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toBool());

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file));
    QVERIFY(Mock::VerifyAndClearExpectations(reply));
    verifyMocks();
}

void
TestDownload::testSslErrorsIgnored() {
    QList<QSslError> errors;
    errors.append(QSslError(QSslError::CertificateExpired));

    auto file = new MockFile("test");
    auto reply = new MockNetworkReply();

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QPair<QString, QString> encodingHeader("Accept-Encoding",
        QString("identity"));

    EXPECT_CALL(*_reqFactory, get(RequestHasHeader(encodingHeader)))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply, canIgnoreSslErrors(errors))
        .Times(1)
        .WillOnce(Return(true));

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, remove())
        .Times(0);

    EXPECT_CALL(*file, close())
        .Times(1);

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);

    download->start();  // change state
    download->startDownload();

    QSignalSpy stateSpy(download, SIGNAL(stateChanged()));
    reply->sslErrors(errors);

    QCOMPARE(0, stateSpy.count());  // we did not set it to error

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file));
    QVERIFY(Mock::VerifyAndClearExpectations(reply));
    verifyMocks();
}

void
TestDownload::testSslErrorsNotIgnored() {
    QList<QSslError> errors;
    errors.append(QSslError(QSslError::CertificateExpired));

    QScopedPointer<MockFile> file(new MockFile("test"));
    QScopedPointer<MockNetworkReply> reply(new MockNetworkReply());

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QPair<QString, QString> encodingHeader("Accept-Encoding",
        QString("identity"));

    EXPECT_CALL(*_reqFactory, get(RequestHasHeader(encodingHeader)))
        .Times(1)
        .WillOnce(Return(reply.data()));

    EXPECT_CALL(*reply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply.data(), canIgnoreSslErrors(errors))
        .Times(1)
        .WillOnce(Return(false));

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file.data()));

    EXPECT_CALL(*file.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file.data(), remove())
        .Times(1)
        .WillOnce(Return(true));

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);

    download->start();  // change state
    download->startDownload();

    QSignalSpy stateSpy(download, SIGNAL(stateChanged()));

    reply->sslErrors(errors);

    QCOMPARE(1, stateSpy.count());
    QCOMPARE(Download::ERROR, download->state());

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(reply.data()));
    verifyMocks();
}

void
TestDownload::testLocalPathConfined() {
    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    // assert that the root path used is not the one in the metadata
    QVariantMap metadata;
    QString localPath = "/home/my/local/path";
    metadata["local-path"] = localPath;

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, true,
        _rootPath, _url, metadata, _headers));

    QVERIFY(download->filePath() != localPath);
    verifyMocks();
}

void
TestDownload::testLocalPathNotConfined() {
    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QVariantMap metadata;
    QString localPath = "/home/my/local/path";
    metadata["local-path"] = localPath;

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, false,
        _rootPath, _url, metadata, _headers));

    QCOMPARE(download->filePath(), localPath);
    verifyMocks();
}

void
TestDownload::testInvalidUrl() {
    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, QUrl(), _metadata, _headers));

    QVERIFY(!download->isValid());
    verifyMocks();
}

void
TestDownload::testValidUrl() {
    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers));

    QVERIFY(download->isValid());
    verifyMocks();
}

void
TestDownload::testInvalidHashAlgorithm() {
    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, "hash", "not-valid-algo", _metadata,
        _headers));
    QVERIFY(!download->isValid());
    verifyMocks();
}

void
TestDownload::testValidHashAlgorithm_data() {
    QTest::addColumn<QString>("algo");

    QTest::newRow("md5") << "md5";
    QTest::newRow("sha1") << "sha1";
    QTest::newRow("sha224") << "sha224";
    QTest::newRow("sha256") << "sha256";
    QTest::newRow("sha384") << "sha384";
    QTest::newRow("sha512") << "sha512";
    QTest::newRow("Empty string") << "";
}

void
TestDownload::testValidHashAlgorithm() {
    QFETCH(QString, algo);
    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, "hash", algo, _metadata, _headers));
    QVERIFY(download->isValid());
    verifyMocks();
}

void
TestDownload::testInvalidFilePresent() {
    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    // create a file so that we get an error
    QString filePath = testDirectory() + QDir::separator() + "test_file.jpg";
    QScopedPointer<QFile> file(new QFile(filePath));
    file->open(QIODevice::ReadWrite | QFile::Append);
    file->write("data data data!");
    file->close();

    QVariantMap metadata;
    metadata["local-path"] = filePath;

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, false,
        _rootPath, _url, metadata, _headers));
    QVERIFY(!download->isValid());
    verifyMocks();
}

void
TestDownload::testValidFileNotPresent() {
    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QString filePath = testDirectory() + QDir::separator() + "test_file.jpg";

    QVariantMap metadata;
    metadata["local-path"] = filePath;

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, false,
        _rootPath, _url, metadata, _headers));
    QVERIFY(download->isValid());
    verifyMocks();
}

void
TestDownload::testDownloadPresent() {
    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    // create a download and get the filename to use, then write it
    // and create the same download and assert that the filename is diff

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, true,
        _rootPath, _url, _metadata, _headers));

    QString filePath = download->filePath();

    QScopedPointer<QFile> file(new QFile(filePath));
    file->open(QIODevice::ReadWrite | QFile::Append);
    file->write("data data data!");
    file->close();

    QScopedPointer<FileDownload> other(new FileDownload(_id, _path, true,
        _rootPath, _url, _metadata, _headers));

    QVERIFY(filePath != other->filePath());
    verifyMocks();
}

void
TestDownload::testDownloadPresentSeveralFiles_data() {
    QTest::addColumn<int>("count");

    QTest::newRow("One") << 1;
    QTest::newRow("Some") << 3;
    QTest::newRow("Several") << 10;
    QTest::newRow("Plenti") << 100;
}

void
TestDownload::testDownloadPresentSeveralFiles() {
    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    QFETCH(int, count);

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, true,
        _rootPath, _url, _metadata, _headers));

    QString filePath = download->filePath();

    QScopedPointer<QFile> file(new QFile(filePath));
    file->open(QIODevice::ReadWrite | QFile::Append);
    file->write("data data data!");
    file->close();

    QFileInfo fileInfo(filePath);
    auto suffix = "." + fileInfo.completeSuffix();
    qDebug() << "SUFIX" << suffix;
    auto prefix = filePath.left(filePath.size() - suffix.size());
    qDebug() << "PREFIX" << prefix;

    // write the rest of the files
    for(int index=1; index < count; index++) {
        auto otherPath = QString("%1 (%2)%3").arg(prefix).arg(index).arg(suffix);
        qDebug() << "Writing a new file" << otherPath;
        QScopedPointer<QFile> otherFile(new QFile(otherPath));
        otherFile->open(QIODevice::ReadWrite | QFile::Append);
        otherFile->write("data data data!");
        otherFile->close();
    }

    QScopedPointer<FileDownload> other(new FileDownload(_id, _path, true,
        _rootPath, _url, _metadata, _headers));

    QVERIFY(filePath != other->filePath());
    if (count > 0) {
        auto expectedPath = QString("%1 (%2)%3").arg(prefix).arg(count).arg(suffix);
        QCOMPARE(expectedPath, other->filePath());
    }
    verifyMocks();
}

void
TestDownload::testProcessingJustOnce() {
    QString command = "touch";
    QVariantMap metadata;
    metadata["post-download-command"] = command;

    auto file = new MockFile("test");
    auto reply = new MockNetworkReply();
    QScopedPointer<MockProcess> process(new MockProcess());
    QByteArray hashData(100, 'f');
    auto hashString = QString(hashData.toHex());
    auto hash = new MockCryptographicHash();

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply, attribute(_))
        .Times(1)
        .WillOnce(Return(QVariant(200)));

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, reset())
        .Times(1);

    EXPECT_CALL(*file, remove())
        .Times(0);

    EXPECT_CALL(*file, close())
        .Times(1);

    EXPECT_CALL(*file, device())
        .Times(1)
        .WillOnce(Return(nullptr));

    EXPECT_CALL(*_cryptoFactory, createCryptographicHash(_, _))
        .Times(1)
        .WillOnce(Return(hash));

    EXPECT_CALL(*hash, addData(_))
        .Times(1);

    EXPECT_CALL(*hash, result())
        .Times(1)
        .WillOnce(Return(hashData));

    // process factory and process expectation
    EXPECT_CALL(*_processFactory, createProcess())
        .Times(1)
        .WillOnce(Return(process.data()));

    EXPECT_CALL(*process.data(), start(command, _, _))
        .Times(1);

    auto download = new FileDownload(_id,
        _path, _isConfined, _rootPath, _url, hashString, _algo, metadata,
        _headers);

    QSignalSpy processingSpy(download, SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    // makes the process to be executed
    reply->finished();

    QCOMPARE(processingSpy.count(), 1);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(hash));
    QVERIFY(Mock::VerifyAndClearExpectations(file));
    QVERIFY(Mock::VerifyAndClearExpectations(reply));
    verifyMocks();
}

void
TestDownload::testFileSystemErrorProgress() {
    QScopedPointer<MockFile> file(new MockFile("test"));
    QScopedPointer<MockNetworkReply> reply(new MockNetworkReply());

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    // set expectations to get the request and the reply correctly

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply.data()));

    EXPECT_CALL(*reply.data(), readAll())
        .Times(1)
        .WillOnce(Return(QByteArray()));

    EXPECT_CALL(*reply.data(), setReadBufferSize(_))
        .Times(1);

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file.data()));

    EXPECT_CALL(*file.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file.data(), write(_))
        .Times(1)
        .WillOnce(Return(0));

    EXPECT_CALL(*file.data(), flush())
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*file.data(), error())
        .Times(1)
        .WillOnce(Return(QFile::WriteError));  // any error will do

    EXPECT_CALL(*file.data(), remove())
        .Times(1)
        .WillOnce(Return(true));

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);
    QSignalSpy spy(download, SIGNAL(error(QString)));

    download->start();
    download->startDownload();

    reply->downloadProgress(0, 13);  // emit progress so that we try to write

    // assert that the error signal is emitted
    QCOMPARE(spy.count(), 1);
    auto arguments = spy.takeFirst();
    // assert that the size is not the received but the file size
    QCOMPARE(arguments.at(0).toString(),
        QString("FILE SYSTEM ERROR: %1").arg(QFile::WriteError));

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(reply.data()));
    verifyMocks();
}

void
TestDownload::testFileSystemErrorPause() {
    QScopedPointer<MockFile> file(new MockFile("test"));
    QScopedPointer<MockNetworkReply> reply(new MockNetworkReply());

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    // set expectations to get the request and the reply correctly

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply.data()));

    EXPECT_CALL(*reply.data(), readAll())
        .Times(1)
        .WillOnce(Return(QByteArray()));

    EXPECT_CALL(*reply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply.data(), abort())
        .Times(1);

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file.data()));

    EXPECT_CALL(*file.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file.data(), write(_))
        .Times(1)
        .WillOnce(Return(0));

    EXPECT_CALL(*file.data(), flush())
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*file.data(), error())
        .Times(1)
        .WillOnce(Return(QFile::WriteError));  // any error will do

    EXPECT_CALL(*file.data(), remove())
        .Times(1)
        .WillOnce(Return(true));

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);
    QSignalSpy spy(download, SIGNAL(error(QString)));

    download->start();
    download->startDownload();
    download->pause();
    download->pauseDownload();

    // assert that the error signal is emitted
    QCOMPARE(spy.count(), 1);
    auto arguments = spy.takeFirst();
    // assert that the size is not the received but the file size
    QCOMPARE(arguments.at(0).toString(),
        QString("FILE SYSTEM ERROR: %1").arg(QFile::WriteError));

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(reply.data()));
    verifyMocks();
}

void
TestDownload::testRedirectCycle() {
    QUrl redirectUrl("http://one.ubuntu.com");
    QScopedPointer<MockFile> firstFile(new MockFile("test"));
    QScopedPointer<MockFile> secondFile(new MockFile("test"));
    QScopedPointer<MockNetworkReply> firstReply(new MockNetworkReply());
    QScopedPointer<MockNetworkReply> secondReply(new MockNetworkReply());

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(2)
        .WillOnce(Return(firstReply.data()))
        .WillOnce(Return(secondReply.data()));

    EXPECT_CALL(*firstReply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*firstReply.data(),
            attribute(QNetworkRequest::RedirectionTargetAttribute))
        .Times(1)
        .WillOnce(Return(QVariant(redirectUrl)));

    EXPECT_CALL(*secondReply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*secondReply.data(),
            attribute(QNetworkRequest::RedirectionTargetAttribute))
        .Times(1)
        .WillOnce(Return(QVariant(_url)));

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(2)
        .WillOnce(Return(firstFile.data()))
        .WillOnce(Return(secondFile.data()));

    EXPECT_CALL(*firstFile.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*secondFile.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*firstFile.data(), remove())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*secondFile.data(), remove())
        .Times(1)
        .WillOnce(Return(true));

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);
    QSignalSpy errorSpy(download, SIGNAL(error(QString)));
    QSignalSpy networkErrorSpy(download,
        SIGNAL(networkError(NetworkErrorStruct)));

    download->start();  // change state
    download->startDownload();

    // emit finished and an errors should be emitted
    firstReply->finished();
    secondReply->finished();

    QTRY_COMPARE(networkErrorSpy.count(), 1);
    QTRY_COMPARE(errorSpy.count(), 1);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(firstFile.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(secondFile.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(firstReply.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(secondReply.data()));
    verifyMocks();
}

void
TestDownload::testSingleRedirect() {
    QUrl redirectUrl("http://one.ubuntu.com");
    QScopedPointer<MockFile> firstFile(new MockFile("test"));
    auto secondFile = new MockFile("test");
    QScopedPointer<MockNetworkReply> firstReply(new MockNetworkReply());
    QScopedPointer<MockNetworkReply> secondReply(new MockNetworkReply());

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(2)
        .WillOnce(Return(firstReply.data()))
        .WillOnce(Return(secondReply.data()));

    EXPECT_CALL(*firstReply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*firstReply.data(),
            attribute(QNetworkRequest::RedirectionTargetAttribute))
        .Times(1)
        .WillOnce(Return(QVariant(redirectUrl)));

    EXPECT_CALL(*secondReply.data(), setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*secondReply.data(),
            attribute(QNetworkRequest::RedirectionTargetAttribute))
        .Times(1)
        .WillOnce(Return(QVariant()));

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(2)
        .WillOnce(Return(firstFile.data()))
        .WillOnce(Return(secondFile));

    EXPECT_CALL(*firstFile.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*secondFile, open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*firstFile.data(), remove())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*secondFile, remove())
        .Times(0);

    EXPECT_CALL(*secondFile, close())
        .Times(1);

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers);

    QSignalSpy errorSpy(download, SIGNAL(error(QString)));
    QSignalSpy networkErrorSpy(download,
        SIGNAL(networkError(NetworkErrorStruct)));
    QSignalSpy finishedSpy(download, SIGNAL(finished(QString)));

    download->start();
    download->startDownload();

    firstReply->finished();
    secondReply->finished();

    QTRY_COMPARE(networkErrorSpy.count(), 0);
    QTRY_COMPARE(errorSpy.count(), 0);
    QTRY_COMPARE(finishedSpy.count(), 1);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(firstFile.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(secondFile));
    QVERIFY(Mock::VerifyAndClearExpectations(firstReply.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(secondReply.data()));
    verifyMocks();
}

void
TestDownload::testRedirectDoesNotUnlockPath() {
    auto mutex = new MockFileNameMutex();
    auto path = QString("random path");
    FileNameMutex::deleteInstance();
    FileNameMutex::setInstance(mutex);

    // set the expectations of the filename mutex and rerun the test
    EXPECT_CALL(*mutex, lockFileName(_))
        .Times(1)
        .WillOnce(Return(path));

    EXPECT_CALL(*mutex, unlockFileName(path))
        .Times(1); // assert that is not unlocked more than once

    testSingleRedirect();

    QVERIFY(Mock::VerifyAndClearExpectations(mutex));
    FileNameMutex::deleteInstance();
}

void
TestDownload::testCancelUnlocksPath() {
    auto mutex = new MockFileNameMutex();
    auto path = QString("random path");
    FileNameMutex::deleteInstance();
    FileNameMutex::setInstance(mutex);

    // set the expectations of the filename mutex and rerun the test
    EXPECT_CALL(*mutex, lockFileName(_))
        .Times(1)
        .WillOnce(Return(path));

    EXPECT_CALL(*mutex, unlockFileName(path))
        .Times(1); // assert that is not unlocked more than once

    testCancelDownload();

    QVERIFY(Mock::VerifyAndClearExpectations(mutex));
    FileNameMutex::deleteInstance();
}

void
TestDownload::testFinishUnlocksPath() {
    auto mutex = new MockFileNameMutex();
    auto path = QString("random path");
    FileNameMutex::deleteInstance();
    FileNameMutex::setInstance(mutex);

    // set the expectations of the filename mutex and rerun the test
    EXPECT_CALL(*mutex, lockFileName(_))
        .Times(1)
        .WillOnce(Return(path));

    EXPECT_CALL(*mutex, unlockFileName(path))
        .Times(1); // assert that is not unlocked more than once

    testOnSuccessNoHash();

    QVERIFY(Mock::VerifyAndClearExpectations(mutex));
    FileNameMutex::deleteInstance();
}

void
TestDownload::testProcessFinishUnlocksPath() {
    auto command = QString("touch");
    QVariantMap metadata;
    metadata["post-download-command"] = QStringList(command);
    auto mutex = new MockFileNameMutex();
    auto path = QString("random path");
    FileNameMutex::deleteInstance();
    FileNameMutex::setInstance(mutex);
    QScopedPointer<MockFile> file(new MockFile("test"));
    auto reply = new MockNetworkReply();
    QScopedPointer<MockProcess> process(new MockProcess());

    // set the expectations of the filename mutex and rerun the test
    EXPECT_CALL(*mutex, lockFileName(_))
        .Times(1)
        .WillOnce(Return(path));

    EXPECT_CALL(*mutex, unlockFileName(path))
        .Times(1); // assert that is not unlocked more than once

    // write the expectations of the reply which is what we are
    // really testing

    EXPECT_CALL(*_networkInfo, isOnline())
        .WillRepeatedly(Return(true));

    EXPECT_CALL(*_reqFactory, get(_))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    EXPECT_CALL(*reply, attribute(_))
        .Times(1)
        .WillOnce(Return(QVariant(200)));

    // file system expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file.data()));

    EXPECT_CALL(*file.data(), open(QIODevice::ReadWrite | QFile::Append))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file.data(), remove())
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*_cryptoFactory, createCryptographicHash(_, _))
        .Times(0);

    // process factory and process expectation
    EXPECT_CALL(*_processFactory, createProcess())
        .Times(1)
        .WillOnce(Return(process.data()));

    EXPECT_CALL(*process.data(), start(command, _, _))
        .Times(1);

    auto download = new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, metadata, _headers);

    QSignalSpy spy(download, SIGNAL(finished(QString)));
    QSignalSpy processingSpy(download, SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    // emit the finish signal and expect it to be raised
    emit reply->finished();
    emit process->finished(0, QProcess::NormalExit);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(processingSpy.count(), 1);
    QCOMPARE(download->state(), Download::FINISH);

    delete download;

    QVERIFY(Mock::VerifyAndClearExpectations(file.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(reply));
    QVERIFY(Mock::VerifyAndClearExpectations(process.data()));
    QVERIFY(Mock::VerifyAndClearExpectations(mutex));
    verifyMocks();
    FileNameMutex::deleteInstance();
}

void
TestDownload::testErrorUnlocksPath() {
    auto mutex = new MockFileNameMutex();
    auto path = QString("random path");
    FileNameMutex::deleteInstance();
    FileNameMutex::setInstance(mutex);

    // set the expectations of the filename mutex and rerun the test
    EXPECT_CALL(*mutex, lockFileName(_))
        .Times(1)
        .WillOnce(Return(path));

    EXPECT_CALL(*mutex, unlockFileName(path))
        .Times(1); // assert that is not unlocked more than once

    testOnSslError();

    QVERIFY(Mock::VerifyAndClearExpectations(mutex));
    FileNameMutex::deleteInstance();
}

void
TestDownload::testLockCustomLocalPath() {
    auto mutex = new MockFileNameMutex();
    auto path = QString("random path");
    FileNameMutex::deleteInstance();
    FileNameMutex::setInstance(mutex);

    // set the expectations of the filename mutex and rerun the test
    EXPECT_CALL(*mutex, lockFileName(QStringEndsWith(".tmp")))
        .Times(1)
        .WillOnce(Return(path));

    testLocalPathNotConfined();

    QVERIFY(Mock::VerifyAndClearExpectations(mutex));
    FileNameMutex::deleteInstance();
}

QTEST_MAIN(TestDownload)
