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
#include <ubuntu/transfers/system/hash_algorithm.h>
#include <ubuntu/transfers/system/uuid_utils.h>
#include <ubuntu/download_manager/tests/server/network_reply.h>
#include <ubuntu/download_manager/tests/server/process.h>
#include "test_download.h"

using namespace Ubuntu::DownloadManager::Daemon;

TestDownload::TestDownload(QObject* parent)
    : BaseTestCase("TestDownload", parent) {
}

void
TestDownload::init() {
    BaseTestCase::init();

    _id = UuidUtils::getDBusString(QUuid::createUuid());
    _isConfined = false;
    _rootPath = testDirectory();
    _path = "random path to dbus";
    _url = QUrl("http://ubuntu.com");
    _algo = "Sha256";
    _networkInfo = new FakeSystemNetworkInfo();
    _networkInfo->setOnline(true);
    SystemNetworkInfo::setInstance(_networkInfo);
    _reqFactory = new FakeRequestFactory();
    RequestFactory::setInstance(_reqFactory);
    _processFactory = new FakeProcessFactory();
    ProcessFactory::setInstance(_processFactory);
    _fileManager = new FakeFileManager();
    FileManager::setInstance(_fileManager);
    _fileNameMutex = new FakeFileNameMutex();
    FileNameMutex::setInstance(_fileNameMutex);
}

void
TestDownload::cleanup() {
    BaseTestCase::cleanup();

    SystemNetworkInfo::deleteInstance();
    RequestFactory::deleteInstance();
    ProcessFactory::deleteInstance();
    FileManager::deleteInstance();
    FileNameMutex::deleteInstance();
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

    QScopedPointer<FileDownload> download(new FileDownload(id, path, _isConfined,
        _rootPath, url, _metadata, _headers));

    // assert that we did set the initial state correctly
    // gets for internal state

    QCOMPARE(download->downloadId(), id);
    QCOMPARE(download->path(), path);
    QCOMPARE(download->url(), url);
    QCOMPARE(download->state(), Download::IDLE);
    QCOMPARE(download->progress(), 0ULL);
    QCOMPARE(download->totalSize(), 0ULL);
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
    QScopedPointer<FileDownload> download(new FileDownload(_id, path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QCOMPARE(download->path(), path);
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
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, url, _metadata, _headers));
    QCOMPARE(download->url(), url);
}

void
TestDownload::testProgress_data() {
    QTest::addColumn<QByteArray>("fileData");
    QTest::addColumn<qulonglong>("received");
    QTest::addColumn<qulonglong>("total");

    QTest::newRow("First row") << QByteArray(0, 'f') << 67ULL << 200ULL;
    QTest::newRow("Second row") << QByteArray(200, 's') << 45ULL << 12000ULL;
    QTest::newRow("Third row") << QByteArray(300, 't') << 2ULL << 2345ULL;
    QTest::newRow("Last row") << QByteArray(400, 'l') << 3434ULL << 2323ULL;
}

void
TestDownload::testProgress() {
    QFETCH(QByteArray, fileData);
    QFETCH(qulonglong, received);
    QFETCH(qulonglong, total);

    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(progress(qulonglong, qulonglong)));

    // start the download so that we do have access to the reply
    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);
    reply->setData(fileData);
    emit reply->downloadProgress(received, total);

    // assert that the total is set and that the signals is emitted
    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 1, 20000);
    QCOMPARE(download->totalSize(), total);

    QList<QVariant> arguments = spy.takeFirst();
    // assert that the size is not the received but the file size
    QCOMPARE(arguments.at(0).toULongLong(), (qulonglong)fileData.size());
    QCOMPARE(arguments.at(1).toULongLong(), total);
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

    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(progress(qulonglong, qulonglong)));

    // start the download so that we do have access to the reply
    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);
    reply->setData(fileData);
    emit reply->downloadProgress(received, total);

    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 1, 20000);

    QList<QVariant> arguments = spy.takeFirst();
    qulonglong size = (qulonglong)fileData.size();
    QCOMPARE(arguments.at(0).toULongLong(), size);
    // must be the same as the progress
    QCOMPARE(arguments.at(1).toULongLong(), size);
}

void
TestDownload::testTotalSize() {
    qulonglong received = 30ULL;
    qulonglong total = 200ULL;

    // assert that the total size is just set once
    // by emitting two signals with diff sizes
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(progress(qulonglong, qulonglong)));

    // start the download so that we do have access to the reply
    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    emit reply->downloadProgress(received, total);
    emit reply->downloadProgress(received, 2*total);

    QTRY_COMPARE_WITH_TIMEOUT(download->totalSize(), total, 20000);
    QCOMPARE(spy.count(), 2);
}

void
TestDownload::testTotalSizeNoProgress() {
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QCOMPARE(0ULL, download->totalSize());
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
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    download->setThrottle(speed);
    QCOMPARE(speed, download->throttle());
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

    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(throttleChanged()));
    download->setThrottle(speed);

    QTRY_COMPARE(1, spy.count());

    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);
    calledMethods = reply->calledMethods();

    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("setReadBufferSize"), calledMethods[0].methodName());
    QCOMPARE(speed,
        (reinterpret_cast<UintWrapper*>(
             calledMethods[0].params().inParams()[0]))->value());
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

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    download->allowGSMDownload(value);
    QSignalSpy spy(download.data(), SIGNAL(stateChanged()));

    download->allowGSMDownload(value);
    QCOMPARE(spy.count(), 0);
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

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    download->allowGSMDownload(oldValue);
    QSignalSpy spy(download.data(), SIGNAL(stateChanged()));

    download->allowGSMDownload(newValue);
    QCOMPARE(spy.count(), 1);
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
    _networkInfo->setMode(mode.value<QNetworkInfo::NetworkMode>());
    _networkInfo->record();

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    download->allowGSMDownload(true);
    QVERIFY(download->canDownload());
    QList<MethodData> calledMethods = _networkInfo->calledMethods();
    QCOMPARE(1, calledMethods.count());
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
    _networkInfo->setMode(mode.value<QNetworkInfo::NetworkMode>());
    _networkInfo->record();

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    download->allowGSMDownload(false);

    QCOMPARE(result, download->canDownload());
    QList<MethodData> calledMethods = _networkInfo->calledMethods();
    QCOMPARE(1, calledMethods.count());
}

void
TestDownload::testCancel() {
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(stateChanged()));
    download->cancel();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::CANCEL);
}

void
TestDownload::testPause() {
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(stateChanged()));
    download->pause();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::PAUSE);
}

void
TestDownload::testResume() {
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(stateChanged()));
    download->resume();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::RESUME);
}

void
TestDownload::testStart() {
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(stateChanged()));
    download->start();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::START);
}

void
TestDownload::testCancelDownload() {
    // tell the fake nam to record so that we can access the reply

    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(),
        SIGNAL(canceled(bool)));  // NOLINT(readability/function)

    download->start();  // change state
    download->startDownload();
    download->cancel();  // change state
    download->cancelDownload();  // method under test

    // assert that method was indeed called
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // assert that the reply was aborted and deleted via deleteLater
    calledMethods = reply->calledMethods();
    QCOMPARE(2, calledMethods.count());  // setThrottle AND abort

    QCOMPARE(QString("abort"), calledMethods[1].methodName());
    QCOMPARE(spy.count(), 1);

    // assert that the files do not exist in the system
    QFileInfo info = QFileInfo(download->filePath());
    QVERIFY(!info.exists());
}

void
TestDownload::testCancelDownloadNotStarted() {
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(),
        SIGNAL(canceled(bool)));  // NOLINT(readability/function)

    download->cancel();  // change state
    download->cancelDownload();  // method under test

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(0, calledMethods.count());
    QCOMPARE(spy.count(), 1);

    // assert file is not present
    QFileInfo info = QFileInfo(download->filePath());
    QVERIFY(!info.exists());
}

void
TestDownload::testPauseDownload() {
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(),
        SIGNAL(paused(bool)));  // NOLINT(readability/function)

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);
    reply->setData(QByteArray(900, 's'));

    download->pause();  // change state
    download->pauseDownload();  // method under test

    // assert that the reply was aborted and deleted via deleteLater
    calledMethods = reply->calledMethods();
    QCOMPARE(3, calledMethods.count());  // throttle + abort + readAll

    QCOMPARE(QString("abort"), calledMethods[1].methodName());
    QCOMPARE(QString("readAll"), calledMethods[2].methodName());

    // assert current size is correct (progress)
    QCOMPARE(download->progress(), (qulonglong)reply->data().size());

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toBool());
}

void
TestDownload::testPauseDownloadNotStarted() {
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(),
        SIGNAL(paused(bool)));  // NOLINT(readability/function)

    download->pause();
    download->pauseDownload();

    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(!arguments.at(0).toBool());
}

void
TestDownload::testResumeRunning() {
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(),
        SIGNAL(resumed(bool)));  // NOLINT(readability/function)

    download->start();
    download->startDownload();
    download->resume();
    download->resumeDownload();

    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(!arguments.at(0).toBool());
}

void
TestDownload::testResumeDownload() {
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(paused(bool result)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);
    reply->setData(QByteArray(900, 's'));

    download->pause();  // change state
    download->pauseDownload();  // method under test

    // clear the called methods from the reqFactory
    _reqFactory->clear();
    download->resume();
    download->resumeDownload();

    // get the info for the second created request
    calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    RequestWrapper* requestWrapper = reinterpret_cast<RequestWrapper*>(
        calledMethods[0].params().inParams()[0]);
    QNetworkRequest request = requestWrapper->request();
    // assert that the request has the correct headers set
    QVERIFY(request.hasRawHeader("Range"));
    QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(
        reply->data().size()) + "-";
    QCOMPARE(rangeHeaderValue, request.rawHeader("Range"));
}

void
TestDownload::testStartDownload() {
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(),
        SIGNAL(started(bool)));  // NOLINT(readability/function)

    download->start();  // change state
    download->startDownload();

    // assert that method was indeed called
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(spy.count(), 1);

    // assert that the files does exist in the system
    QFileInfo info(download->filePath());
    QVERIFY(!info.exists());
    info = QFileInfo(download->filePath() + ".tmp");
    QVERIFY(info.exists());
}

void
TestDownload::testStartDownloadAlreadyStarted() {
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(),
        SIGNAL(started(bool)));  // NOLINT(readability/function)

    download->start();  // change state
    download->startDownload();
    download->startDownload();  // second redundant call under test

    // assert that method was indeed called
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());  // if the reqFactory is called
                                         // twice we have a bug
    QCOMPARE(spy.count(), 2);  // signal should be raised twice

    // assert that the files does exist in the system
    QFileInfo info = QFileInfo(download->filePath());
    QVERIFY(!info.exists());
    info = QFileInfo(download->filePath() + ".tmp");
    QVERIFY(info.exists());
}

void
TestDownload::testOnSuccessNoHash() {
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(finished(QString)));
    QSignalSpy processingSpy(download.data(), SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // emit the finish signal and expect it to be raised
    emit reply->finished();
    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 1, 20000);
    QTRY_COMPARE_WITH_TIMEOUT(processingSpy.count(), 0, 20000);
    QCOMPARE(download->state(), Download::FINISH);
}

void
TestDownload::testOnSuccessHashError() {
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, "imposible-hash-is-not-hex", _algo, _metadata, _headers));
    QSignalSpy errorSpy(download.data(), SIGNAL(error(QString)));
    QSignalSpy processingSpy(download.data(), SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);
    reply->setData(QByteArray(200, 'f'));

    download->pause();
    download->pauseDownload();   // write the data in the internal storage

    // clear the called methods from the reqFactory
    _reqFactory->clear();
    download->resume();
    download->resumeDownload();

    calledMethods = _reqFactory->calledMethods();
    reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // create the spy here else we will register other not interesting
    // signals
    QSignalSpy stateSpy(download.data(), SIGNAL(stateChanged()));
    emit reply->finished();

    // the has is a random string so we should get an error signal
    QTRY_COMPARE_WITH_TIMEOUT(errorSpy.count(), 1, 20000);
    QTRY_COMPARE_WITH_TIMEOUT(stateSpy.count(), 1, 20000);
    QTRY_COMPARE_WITH_TIMEOUT(processingSpy.count(), 1, 20000);
    QCOMPARE(download->state(), Download::ERROR);
}

void
TestDownload::testOnSuccessHash_data() {
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QString>("hash");

    QByteArray firstData(0, 'f');
    QByteArray secondData(200, 's');
    QByteArray thirdData(300, 't');
    QByteArray lastData(400, 'l');
    QCryptographicHash::Algorithm algorithm =
        HashAlgorithm::getHashAlgo(_algo);

    QTest::newRow("First row") << firstData
        << QString(QCryptographicHash::hash(firstData, algorithm).toHex());
    QTest::newRow("Second row") << secondData
        << QString(QCryptographicHash::hash(secondData, algorithm).toHex());
    QTest::newRow("Third row") << thirdData
        << QString(QCryptographicHash::hash(thirdData, algorithm).toHex());
    QTest::newRow("Last row") << lastData
        << QString(QCryptographicHash::hash(lastData, algorithm).toHex());
}

void
TestDownload::testOnSuccessHash() {
    QFETCH(QByteArray, data);
    QFETCH(QString, hash);

    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, hash, _algo, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(finished(QString)));
    QSignalSpy processingSpy(download.data(), SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);
    reply->setData(data);

    download->pause();
    download->pauseDownload();   // write the data in the internal storage

    // clear the called methods from the reqFactory
    _reqFactory->clear();
    download->resume();
    download->resumeDownload();

    calledMethods = _reqFactory->calledMethods();
    reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);
    emit reply->finished();

    // the hash should be correct and we should get the finish signal
    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 1, 20000);
    QTRY_COMPARE_WITH_TIMEOUT(processingSpy.count(), 1, 20000);
    QCOMPARE(download->state(), Download::FINISH);
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

    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy errorSpy(download.data(), SIGNAL(error(QString)));
    QSignalSpy httpErrorSpy(download.data(), SIGNAL(httpError(HttpErrorStruct)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // set the attrs in the reply so that we do raise two signals
    reply->setAttribute(QNetworkRequest::HttpStatusCodeAttribute, code);
    reply->setAttribute(QNetworkRequest::HttpReasonPhraseAttribute, message);

    // emit the error and ensure that the signals are raised
    reply->emitHttpError(QNetworkReply::ContentAccessDenied);
    QCOMPARE(httpErrorSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 1);
}

void
TestDownload::testOnSslError() {
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(error(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    QList<QSslError> errors;
    emit reply->sslErrors(errors);
    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 1, 20000);
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

    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy errorSpy(download.data(), SIGNAL(error(QString)));
    QSignalSpy networkErrorSpy(download.data(),
        SIGNAL(networkError(NetworkErrorStruct)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // set the attrs in the reply so that we do raise two signals
    reply->clearAttribute(QNetworkRequest::HttpStatusCodeAttribute);

    // emit the error and ensure that the signals are raised
    reply->emitHttpError((QNetworkReply::NetworkError)code);
    QCOMPARE(networkErrorSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 1);
}

void
TestDownload::testOnAuthError() {
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy errorSpy(download.data(), SIGNAL(error(QString)));
    QSignalSpy authErrorSpy(download.data(),
        SIGNAL(authError(AuthErrorStruct)));
    QSignalSpy networkErrorSpy(download.data(),
        SIGNAL(networkError(NetworkErrorStruct)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // emit the error and ensure that the signals are raised
    reply->emitHttpError(QNetworkReply::AuthenticationRequiredError);
    QCOMPARE(networkErrorSpy.count(), 0);
    QCOMPARE(authErrorSpy.count(), 1);
    auto error = authErrorSpy.takeFirst().at(0).value<AuthErrorStruct>();
    QVERIFY(error.getType() == AuthErrorStruct::Server);
    QCOMPARE(errorSpy.count(), 1);
}

void
TestDownload::testOnProxyAuthError() {
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy errorSpy(download.data(), SIGNAL(error(QString)));
    QSignalSpy authErrorSpy(download.data(),
        SIGNAL(authError(AuthErrorStruct)));
    QSignalSpy networkErrorSpy(download.data(),
        SIGNAL(networkError(NetworkErrorStruct)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // emit the error and ensure that the signals are raised
    reply->emitHttpError(QNetworkReply::ProxyAuthenticationRequiredError);
    QCOMPARE(networkErrorSpy.count(), 0);
    QCOMPARE(authErrorSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 1);
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
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, headers));

    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    RequestWrapper* requestWrapper = reinterpret_cast<RequestWrapper*>(
        calledMethods[0].params().inParams()[0]);
    QNetworkRequest request = requestWrapper->request();

    // assert that all headers are present
    foreach(const QString& header, headers.keys()) {
        QByteArray headerName = header.toUtf8();
        QVERIFY(request.hasRawHeader(headerName));
        QCOMPARE(headers[header].toUtf8(), request.rawHeader(headerName));
    }
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
    // similar to the previous test but we want to ensure that range is not set
    QFETCH(StringMap, headers);
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, headers));

    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    RequestWrapper* requestWrapper = reinterpret_cast<RequestWrapper*>(
        calledMethods[0].params().inParams()[0]);
    QNetworkRequest request = requestWrapper->request();

    // assert that range is not present
    QVERIFY(!request.hasRawHeader("Range"));
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

    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, headers));
    QSignalSpy spy(download.data(), SIGNAL(paused(bool result)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);
    reply->setData(QByteArray(900, 's'));

    download->pause();  // change state
    download->pauseDownload();  // method under test

    // clear the called methods from the reqFactory
    _reqFactory->clear();
    download->resume();
    download->resumeDownload();

    // get the info for the second created request
    calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    RequestWrapper* requestWrapper = reinterpret_cast<RequestWrapper*>(
        calledMethods[0].params().inParams()[0]);
    QNetworkRequest request = requestWrapper->request();

    // assert that the request has the correct range header set
    QVERIFY(request.hasRawHeader("Range"));
    QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(
        reply->data().size()) + "-";
    QCOMPARE(rangeHeaderValue, request.rawHeader("Range"));

    // assert that it has the rest of the headers
    foreach(const QString& header, headers.keys()) {
        QByteArray headerName = header.toUtf8();
        QVERIFY(request.hasRawHeader(headerName));
        QCOMPARE(headers[header].toUtf8(), request.rawHeader(headerName));
    }
}

void
TestDownload::testSetRawHeadersWithRangeResume_data() {
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
TestDownload::testSetRawHeadersWithRangeResume() {
    // same as the previous test but we assert that range is correctly set
    QFETCH(StringMap, headers);

    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, headers));
    QSignalSpy spy(download.data(), SIGNAL(paused(bool result)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);
    reply->setData(QByteArray(900, 's'));

    download->pause();  // change state
    download->pauseDownload();  // method under test

    // clear the called methods from the reqFactory
    _reqFactory->clear();
    download->resume();
    download->resumeDownload();

    // get the info for the second created request
    calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    RequestWrapper* requestWrapper = reinterpret_cast<RequestWrapper*>(
        calledMethods[0].params().inParams()[0]);
    QNetworkRequest request = requestWrapper->request();

    // assert that the request has the correct range header set
    QVERIFY(request.hasRawHeader("Range"));
    QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(
        reply->data().size()) + "-";
    QCOMPARE(rangeHeaderValue, request.rawHeader("Range"));
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

    _processFactory->record();
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, metadata, _headers));
    QSignalSpy processingSpy(download.data(), SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // makes the process to be executed
    reply->emitFinished();
    QTRY_COMPARE_WITH_TIMEOUT(processingSpy.count(), 1, 20000);

    calledMethods = _processFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeProcess* process = reinterpret_cast<FakeProcess*>(
        calledMethods[0].params().outParams()[0]);

    calledMethods = process->calledMethods();
    StringWrapper* stringWrapper = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().inParams()[0]);
    QString processCommand = stringWrapper->value();
    StringListWrapper* listWrapper = reinterpret_cast<StringListWrapper*>(
        calledMethods[0].params().inParams()[1]);
    QStringList processArgs = listWrapper->value();
    QCOMPARE(processCommand, command);
    QCOMPARE(0, processArgs.count());
}

void
TestDownload::testProcessExecutedWithParams_data() {
    QTest::addColumn<QString>("command");
    QTest::addColumn<QVariantMap>("metadata");
    QVariantMap first, second, third;
    QStringList firstCommand, secondCommand, thirdCommand;

    firstCommand << "touch" << "test-file";
    first["post-download-command"] = firstCommand;

    QTest::newRow("First row") << firstCommand[0] << first;

    secondCommand << "sudo" << "apt-get" << "install" << "click";
    second["post-download-command"] = secondCommand;

    QTest::newRow("Second row") << secondCommand[0] << second;

    thirdCommand << "grep" << "." << "-Rn";
    third["post-download-command"] = thirdCommand;

    QTest::newRow("Third row") << thirdCommand[0] << third;
}

void
TestDownload::testProcessExecutedWithParams() {
    QFETCH(QString, command);
    QFETCH(QVariantMap, metadata);

    _processFactory->record();
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, metadata, _headers));
    QSignalSpy processingSpy(download.data(), SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // makes the process to be executed
    reply->emitFinished();
    QTRY_COMPARE_WITH_TIMEOUT(processingSpy.count(), 1, 20000);

    calledMethods = _processFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeProcess* process = reinterpret_cast<FakeProcess*>(
        calledMethods[0].params().outParams()[0]);

    calledMethods = process->calledMethods();
    StringWrapper* stringWrapper = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().inParams()[0]);
    QString processCommand = stringWrapper->value();
    StringListWrapper* listWrapper = reinterpret_cast<StringListWrapper*>(
        calledMethods[0].params().inParams()[1]);
    QStringList processArgs = listWrapper->value();
    QCOMPARE(processCommand, command);
    QVERIFY(0 != processArgs.count());
}

void
TestDownload::testProcessExecutedWithParamsFile_data() {
    QTest::addColumn<QString>("command");
    QTest::addColumn<QVariantMap>("metadata");
    QVariantMap first, second, third;
    QStringList firstCommand, secondCommand, thirdCommand;

    firstCommand << "touch" << "$file";
    first["post-download-command"] = firstCommand;

    QTest::newRow("First row") << firstCommand[0] << first;

    secondCommand << "sudo" << "apt-get" << "install" << "$file";
    second["post-download-command"] = secondCommand;

    QTest::newRow("Second row") << secondCommand[0] << second;

    thirdCommand << "grep" << "$file" << "-Rn";
    third["post-download-command"] = thirdCommand;

    QTest::newRow("Third row") << thirdCommand[0] << third;
}

void
TestDownload::testProcessExecutedWithParamsFile() {
    QFETCH(QString, command);
    QFETCH(QVariantMap, metadata);

    _processFactory->record();
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, metadata, _headers));
    QSignalSpy processingSpy(download.data(), SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // makes the process to be executed
    reply->emitFinished();
    QTRY_COMPARE_WITH_TIMEOUT(processingSpy.count(), 1, 20000);

    calledMethods = _processFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeProcess* process = reinterpret_cast<FakeProcess*>(
        calledMethods[0].params().outParams()[0]);

    calledMethods = process->calledMethods();
    StringWrapper* stringWrapper = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().inParams()[0]);
    QString processCommand = stringWrapper->value();
    StringListWrapper* listWrapper = reinterpret_cast<StringListWrapper*>(
        calledMethods[0].params().inParams()[1]);
    QStringList processArgs = listWrapper->value();
    QCOMPARE(processCommand, command);
    QVERIFY(processArgs.contains(download->filePath()));
    QVERIFY(QFile::exists(download->filePath()));
    QVERIFY(!QFile::exists(download->filePath() + ".tmp"));
}

void
TestDownload::testProcessFinishedNoError() {
    QVariantMap metadata;
    QStringList command;
    command << "grep" << "$file" << "-Rn";
    metadata["post-download-command"] = command;

    _processFactory->record();
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(finished(QString)));
    QSignalSpy processingSpy(download.data(), SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // makes the process to be executed
    reply->emitFinished();

    calledMethods = _processFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeProcess* process = reinterpret_cast<FakeProcess*>(
        calledMethods[0].params().outParams()[0]);

    // emit the finished signal with 0 and ensure that finished is emitted
    process->emitFinished(0, QProcess::NormalExit);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(processingSpy.count(), 1);
}

void
TestDownload::testProcessFinishedWithError() {
    QVariantMap metadata;
    QStringList command;
    command << "grep" << "$file" << "-Rn";
    metadata["post-download-command"] = command;

    _processFactory->record();
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, metadata, _headers));
    QSignalSpy errorSpy(download.data(), SIGNAL(error(QString)));
    QSignalSpy processErrorSpy(download.data(),
        SIGNAL(processError(ProcessErrorStruct)));
    QSignalSpy processingSpy(download.data(), SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // makes the process to be executed
    reply->emitFinished();

    calledMethods = _processFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeProcess* process = reinterpret_cast<FakeProcess*>(
        calledMethods[0].params().outParams()[0]);

    // emit the finished signal with a result > 0 and ensure error is emitted
    process->emitFinished(1, QProcess::NormalExit);
    QCOMPARE(processingSpy.count(), 1);
    QCOMPARE(processErrorSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 1);
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

    QVariantMap metadata;
    QStringList command;
    command << "grep" << "$file" << "-Rn";
    metadata["post-download-command"] = command;

    _processFactory->record();
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, metadata, _headers));
    QSignalSpy errorSpy(download.data(), SIGNAL(error(QString)));
    QSignalSpy processErrorSpy(download.data(),
        SIGNAL(processError(ProcessErrorStruct)));
    QSignalSpy processingSpy(download.data(), SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // makes the process to be executed
    reply->emitFinished();

    calledMethods = _processFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeProcess* process = reinterpret_cast<FakeProcess*>(
        calledMethods[0].params().outParams()[0]);
    process->setStandardOutput(stdOut.toUtf8());
    process->setStandardError(stdErr.toUtf8());

    // emit error signal
    process->emitError((QProcess::ProcessError)code);
    QCOMPARE(processingSpy.count(), 1);
    QCOMPARE(processErrorSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 1);
}

void
TestDownload::testProcessFinishedCrash() {
    QVariantMap metadata;
    QStringList command;
    command << "grep" << "$file" << "-Rn";
    metadata["post-download-command"] = command;

    _processFactory->record();
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(error(QString)));
    QSignalSpy processingSpy(download.data(), SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // makes the process to be executed
    reply->emitFinished();

    calledMethods = _processFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeProcess* process = reinterpret_cast<FakeProcess*>(
        calledMethods[0].params().outParams()[0]);

    // emit the finished signal with a result > 0 and ensure error is emitted
    process->emitFinished(1, QProcess::CrashExit);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(processingSpy.count(), 1);
}

void
TestDownload::testFileRemoveAfterSuccessfulProcess() {
    // assert that the file is indeed removed
    QVariantMap metadata;
    QStringList command;
    command << "grep" << "$file" << "-Rn";
    metadata["post-download-command"] = command;

    _processFactory->record();
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, metadata, _headers));
    QSignalSpy finishedSpy(download.data(), SIGNAL(finished(QString)));

    // write something in the expected file
    QString fileName = download->filePath();
    QScopedPointer<QFile> file(new QFile(fileName));
    file->open(QIODevice::ReadWrite | QFile::Append);
    file->write("my data goes here");
    file->close();

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // makes the process to be executed
    reply->emitFinished();

    calledMethods = _processFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeProcess* process = reinterpret_cast<FakeProcess*>(
        calledMethods[0].params().outParams()[0]);

    // emit the finished signal with a result > 0 and ensure error is emitted
    process->emitFinished(0, QProcess::NormalExit);
    QTRY_COMPARE(1, finishedSpy.count());
    // assert that the file does not longer exist in the system
    QVERIFY(!QFile::exists(fileName));
}

void
TestDownload::testSetRawHeaderAcceptEncoding_data() {
    QTest::addColumn<QMap<QString, QString> >("headers");

    // create a number of headers to assert that they are added in the request
    // and that the value is ignore. We used diff lower-upper combination
    // to ensure that it does not really matter
    StringMap first, second, third;

    // add headers to be added except range
    first["Accept-Encoding"] = "text/plain";

    QTest::newRow("First row") << first;

    second["Accept-encoding"] = "en-US";

    QTest::newRow("Second row") << second;

    third["Accept-encoding"] = "348";

    QTest::newRow("Third row") << third;
}

void
TestDownload::testSetRawHeaderAcceptEncoding() {
    QFETCH(StringMap, headers);
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, headers));

    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    RequestWrapper* requestWrapper = reinterpret_cast<RequestWrapper*>(
        calledMethods[0].params().inParams()[0]);
    QNetworkRequest request = requestWrapper->request();

    QCOMPARE(QString("identity").toUtf8(),
        request.rawHeader("Accept-Encoding"));
}

void
TestDownload::testSslErrorsIgnored() {
    QList<QSslError> errors;
    errors.append(QSslError(QSslError::CertificateExpired));

    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));

    download->start();  // change state
    download->startDownload();

    QSignalSpy stateSpy(download.data(), SIGNAL(stateChanged()));

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);
    reply->setCanIgnoreSslErrors(true);
    reply->emitSslErrors(errors);

    calledMethods = reply->calledMethods();
    // assert last method called is ignoreSslErors
    QCOMPARE(QString("canIgnoreSslErrors"),
        calledMethods[calledMethods.count() -1].methodName());
    QCOMPARE(0, stateSpy.count());  // we did not set it to error
}

void
TestDownload::testSslErrorsNotIgnored() {
    QList<QSslError> errors;
    errors.append(QSslError(QSslError::CertificateExpired));

    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));

    download->start();  // change state
    download->startDownload();

    QSignalSpy stateSpy(download.data(), SIGNAL(stateChanged()));

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);
    reply->setCanIgnoreSslErrors(false);
    reply->emitSslErrors(errors);

    calledMethods = reply->calledMethods();
    // assert last method called is ignoreSslErors
    QCOMPARE(QString("canIgnoreSslErrors"),
        calledMethods[calledMethods.count() -1].methodName());
    QCOMPARE(1, stateSpy.count());  // we did not set it to error
    QCOMPARE(Download::ERROR, download->state());
}

void
TestDownload::testLocalPathConfined() {
    // assert that the root path used is not the one in the metadata
    QVariantMap metadata;
    QString localPath = "/home/my/local/path";
    metadata["local-path"] = localPath;

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, true,
        _rootPath, _url, metadata, _headers));

    qDebug() << download->filePath();
    QVERIFY(download->filePath() != localPath);
}

void
TestDownload::testLocalPathNotConfined() {
    QVariantMap metadata;
    QString localPath = "/home/my/local/path";
    metadata["local-path"] = localPath;

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, false,
        _rootPath, _url, metadata, _headers));

    qDebug() << download->filePath();
    QCOMPARE(download->filePath(), localPath);
}

void
TestDownload::testInvalidUrl() {
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, QUrl(), _metadata, _headers));

    QVERIFY(!download->isValid());
}

void
TestDownload::testValidUrl() {
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));

    QVERIFY(download->isValid());
}

void
TestDownload::testInvalidHashAlgorithm() {
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, "hash", "not-valid-algo", _metadata, _headers));
    QVERIFY(!download->isValid());
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
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, "hash", algo, _metadata, _headers));
    QVERIFY(download->isValid());
}

void
TestDownload::testInvalidFilePresent() {
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
}

void
TestDownload::testValidFileNotPresent() {
    QString filePath = testDirectory() + QDir::separator() + "test_file.jpg";

    QVariantMap metadata;
    metadata["local-path"] = filePath;

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, false,
        _rootPath, _url, metadata, _headers));
    QVERIFY(download->isValid());
}

void
TestDownload::testDownloadPresent() {
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
}

void
TestDownload::testProcessingJustOnce() {
    QCryptographicHash::Algorithm algorithm = HashAlgorithm::getHashAlgo(_algo);
    QByteArray data(300, 't');
    QString hash = QString(QCryptographicHash::hash(data, algorithm).toHex());

    QString command = "touch";
    QVariantMap metadata;
    metadata["post-download-command"] = command;

    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, hash, _algo, metadata, _headers));

    QSignalSpy processingSpy(download.data(), SIGNAL(processing(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);
    reply->setData(data);

    // makes the process to be executed
    reply->emitFinished();

    QCOMPARE(processingSpy.count(), 1);
}

void
TestDownload::testFileSystemErrorProgress() {
    // get the file manager instance and make it return a fake file
    // that will return an error
    QScopedPointer<FakeFile> file(new FakeFile("test_file"));
    file->setError(QFile::WriteError);
    _fileManager->setFile(file.data());

    QByteArray fileData(0, 'f');
    qulonglong received = 67ULL;
    qulonglong total = 200ULL;

    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(error(QString)));

    // start the download so that we do have access to the reply
    download->start();  // change state
    download->startDownload();

    auto calledMethods = _reqFactory->calledMethods();
    auto reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);
    reply->setData(fileData);
    emit reply->downloadProgress(received, total);

    // assert that the error signal is emitted
    QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 1, 20000);
    
    auto arguments = spy.takeFirst();
    // assert that the size is not the received but the file size
    QCOMPARE(arguments.at(0).toString(),
        QString("FILE SYSTEM ERROR: %1").arg(QFile::WriteError));
    _fileManager->setFile(nullptr);
}

void
TestDownload::testFileSystemErrorPause() {
    // get the file manager instance and make it return a fake file
    // that will return an error
    QScopedPointer<FakeFile> file(new FakeFile("test_file"));
    file->setError(QFile::WriteError);
    _fileManager->setFile(file.data());

    QByteArray fileData(0, 'f');

    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(error(QString)));

    // start the download so that we do have access to the reply
    download->start();  // change state
    download->startDownload();

    auto calledMethods = _reqFactory->calledMethods();
    auto reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);
    reply->setData(fileData);

    download->pause();  // change state
    download->pauseDownload();  // method under test

    // assert that the error signal is emitted
    QCOMPARE(spy.count(), 1);
    auto arguments = spy.takeFirst();
    // assert that the size is not the received but the file size
    QCOMPARE(arguments.at(0).toString(),
        QString("FILE SYSTEM ERROR: %1").arg(QFile::WriteError));
    _fileManager->setFile(nullptr);
}

void
TestDownload::testRedirectCycle() {
    // the test works as follows, create a request, pause and set the redirect
    // header, wait until the second request is created via the signal from the
    // request factory and set the redirect to a first url
    QUrl redirectUrl("http://redirect.example.com");
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = qobject_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // set the attr for a redirect to a new url
    // makes the process to be executed
    reply->setAttribute(QNetworkRequest::RedirectionTargetAttribute,
        redirectUrl);
    QSignalSpy replySpy(_reqFactory, SIGNAL(requestCreated(NetworkReply*)));

    // use a spy to wait for the second reply
    reply->emitFinished();

    QTRY_COMPARE(1, replySpy.count());

    reply = qobject_cast<FakeNetworkReply*>(
        replySpy.takeFirst().at(0).value<NetworkReply*>());

    download->pause();

    // set the attr to be a loop
    reply->setAttribute(QNetworkRequest::RedirectionTargetAttribute,
        _url);

    // emit finished and an errors should be emitted
    QSignalSpy errorSpy(download.data(), SIGNAL(error(QString)));
    QSignalSpy networkErrorSpy(download.data(),
        SIGNAL(networkError(NetworkErrorStruct)));

    reply->emitFinished();

    QTRY_COMPARE(networkErrorSpy.count(), 1);
    QTRY_COMPARE(errorSpy.count(), 1);
}

void
TestDownload::testSingleRedirect() {
    // ensure that a single redirect is followed
    QUrl redirectUrl("http://redirect.example.com");
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = qobject_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // set the attr for a redirect to a new url
    // makes the process to be executed
    reply->setAttribute(QNetworkRequest::RedirectionTargetAttribute,
        redirectUrl);
    QSignalSpy replySpy(_reqFactory, SIGNAL(requestCreated(NetworkReply*)));

    // use a spy to wait for the second reply
    reply->emitFinished();

    // ensure that a second request is performed
    QTRY_COMPARE(1, replySpy.count());

    reply = qobject_cast<FakeNetworkReply*>(
        replySpy.takeFirst().at(0).value<NetworkReply*>());

    download->pause();

    QSignalSpy errorSpy(download.data(), SIGNAL(error(QString)));
    QSignalSpy networkErrorSpy(download.data(),
        SIGNAL(networkError(NetworkErrorStruct)));
    QSignalSpy finishedSpy(download.data(), SIGNAL(finished(QString)));

    reply->emitFinished();

    QTRY_COMPARE(networkErrorSpy.count(), 0);
    QTRY_COMPARE(errorSpy.count(), 0);
    QTRY_COMPARE(finishedSpy.count(), 1);
}

void
TestDownload::testSeveralRedirects_data() {
    QTest::addColumn<QStringList>("urls");

    QStringList first;
    first.append("http://first.com/resource");
    first.append("http://first.com/moved_resource");
    first.append("http://first.com/last");
    QTest::newRow("Three redirects") << first;

    QStringList second;
    second.append("http://second.com/resource");
    second.append("http://second.com/moved1");
    second.append("http://second.com/moved2");
    second.append("http://second.com/moved3");
    second.append("http://second.com/moved4");
    QTest::newRow("Five redirects") << second;

    QStringList third;
    third.append("http://third.com/resource");
    third.append("http://third.com/moved1");
    third.append("http://third.com/moved2");
    third.append("http://third.com/moved3");
    third.append("http://third.com/moved4");
    third.append("http://third.com/moved5");
    third.append("http://third.com/moved6");
    QTest::newRow("Seven redirects") << third;

}

void
TestDownload::testSeveralRedirects() {
    QFETCH(QStringList, urls);
    auto redirectCount = 0;
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = qobject_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // redirect all the required times
    foreach(const QString& url, urls) {
        reply->setAttribute(QNetworkRequest::RedirectionTargetAttribute,
            url);
        QSignalSpy replySpy(_reqFactory, SIGNAL(requestCreated(NetworkReply*)));

        // use a spy to wait for the second reply
        reply->emitFinished();

        // ensure that a second request is performed
        QTRY_COMPARE(1, replySpy.count());

        reply = qobject_cast<FakeNetworkReply*>(
            replySpy.takeFirst().at(0).value<NetworkReply*>());
        redirectCount++;
    }

    download->pause();

    QSignalSpy errorSpy(download.data(), SIGNAL(error(QString)));
    QSignalSpy networkErrorSpy(download.data(),
        SIGNAL(networkError(NetworkErrorStruct)));
    QSignalSpy finishedSpy(download.data(), SIGNAL(finished(QString)));

    reply->emitFinished();

    QTRY_COMPARE(networkErrorSpy.count(), 0);
    QTRY_COMPARE(errorSpy.count(), 0);
    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(redirectCount, urls.count());
}

void
TestDownload::testRedirectDoesNotUnlockPath() {
    _fileNameMutex->record();

    QUrl redirectUrl("http://redirect.example.com");
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers));

    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = qobject_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // set the attr for a redirect to a new url
    // makes the process to be executed
    reply->setAttribute(QNetworkRequest::RedirectionTargetAttribute,
        redirectUrl);
    QSignalSpy replySpy(_reqFactory, SIGNAL(requestCreated(NetworkReply*)));

    // use a spy to wait for the second reply
    reply->emitFinished();

    // ensure that a second request is performed
    QTRY_COMPARE(1, replySpy.count());

    reply = qobject_cast<FakeNetworkReply*>(
        replySpy.takeFirst().at(0).value<NetworkReply*>());

    download->pause();

    QSignalSpy errorSpy(download.data(), SIGNAL(error(QString)));
    QSignalSpy networkErrorSpy(download.data(),
        SIGNAL(networkError(NetworkErrorStruct)));
    QSignalSpy finishedSpy(download.data(), SIGNAL(finished(QString)));

    reply->emitFinished();

    QTRY_COMPARE(networkErrorSpy.count(), 0);
    QTRY_COMPARE(errorSpy.count(), 0);
    QTRY_COMPARE(finishedSpy.count(), 1);

    // ensure that we only called lock and unlock once and not several times
    calledMethods = _fileNameMutex->calledMethods();
    QCOMPARE(2, calledMethods.count());
    auto methodName = calledMethods[0].methodName();
    QCOMPARE(QString("lockFileName"), methodName);
    auto path = qobject_cast<StringWrapper*>(
        calledMethods[0].params().inParams()[0])->value();
    QCOMPARE(download->filePath(), path);
    methodName = calledMethods[1].methodName();
    QCOMPARE(QString("unlockFileName"), methodName);
    path = qobject_cast<StringWrapper*>(
        calledMethods[1].params().inParams()[0])->value();
    QCOMPARE(download->filePath(), path);
}

void
TestDownload::testCancelUnlocksPath() {
    _fileNameMutex->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(),
        SIGNAL(canceled(bool)));  // NOLINT(readability/function)

    download->start();  // change state
    download->startDownload();
    download->cancel();  // change state
    download->cancelDownload();  // method under test

    // assert that the filename was correctly managed
    auto calledMethods = _fileNameMutex->calledMethods();
    QCOMPARE(2, calledMethods.count());
    auto methodName = calledMethods[0].methodName();
    QCOMPARE(QString("lockFileName"), methodName);
    auto path = qobject_cast<StringWrapper*>(
        calledMethods[0].params().inParams()[0])->value();
    QCOMPARE(download->filePath(), path);
    methodName = calledMethods[1].methodName();
    QCOMPARE(QString("unlockFileName"), methodName);
    path = qobject_cast<StringWrapper*>(
        calledMethods[1].params().inParams()[0])->value();
    QCOMPARE(download->filePath(), path);
}

void
TestDownload::testFinishUnlocksPath() {
    _fileNameMutex->record();
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(finished(QString)));

    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // emit the finish signal and expect it to be raised
    emit reply->finished();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::FINISH);

    calledMethods = _fileNameMutex->calledMethods();
    QCOMPARE(2, calledMethods.count());
    auto methodName = calledMethods[0].methodName();
    QCOMPARE(QString("lockFileName"), methodName);
    auto path = qobject_cast<StringWrapper*>(
        calledMethods[0].params().inParams()[0])->value();
    QCOMPARE(download->filePath(), path);
    methodName = calledMethods[1].methodName();
    QCOMPARE(QString("unlockFileName"), methodName);
    path = qobject_cast<StringWrapper*>(
        calledMethods[1].params().inParams()[0])->value();
    QCOMPARE(download->filePath(), path);
}

void
TestDownload::testProcessFinishUnlocksPath() {
    _fileNameMutex->record();
    _processFactory->record();
    _reqFactory->record();

    QString command = "cd";
    QVariantMap metadata;
    metadata["post-download-command"] = command;

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(finished(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // makes the process to be executed
    reply->emitFinished();

    calledMethods = _processFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeProcess* process = reinterpret_cast<FakeProcess*>(
        calledMethods[0].params().outParams()[0]);

    process->emitFinished(0, QProcess::NormalExit);
    QTRY_COMPARE(spy.count(), 1);

    calledMethods = _fileNameMutex->calledMethods();
    QCOMPARE(2, calledMethods.count());
    auto methodName = calledMethods[0].methodName();
    QCOMPARE(QString("lockFileName"), methodName);
    auto path = qobject_cast<StringWrapper*>(
        calledMethods[0].params().inParams()[0])->value();
    QCOMPARE(download->filePath(), path);
    methodName = calledMethods[1].methodName();
    QCOMPARE(QString("unlockFileName"), methodName);
    path = qobject_cast<StringWrapper*>(
        calledMethods[1].params().inParams()[0])->value();
    QCOMPARE(download->filePath(), path);
}

void
TestDownload::testErrorUnlocksPath() {
    // fake an error and make sure that unlock is called
    _fileNameMutex->record();
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, _metadata, _headers));
    QSignalSpy errorSpy(download.data(), SIGNAL(error(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // set the attrs in the reply so that we do raise two signals
    reply->setAttribute(QNetworkRequest::HttpStatusCodeAttribute, 404);
    reply->setAttribute(QNetworkRequest::HttpReasonPhraseAttribute, "");

    // emit the error and esure that the signals are raised
    reply->emitHttpError(QNetworkReply::ContentAccessDenied);
    QCOMPARE(errorSpy.count(), 1);

    calledMethods = _fileNameMutex->calledMethods();
    QCOMPARE(2, calledMethods.count());
    auto methodName = calledMethods[0].methodName();
    QCOMPARE(QString("lockFileName"), methodName);
    auto path = qobject_cast<StringWrapper*>(
        calledMethods[0].params().inParams()[0])->value();
    QCOMPARE(download->filePath(), path);
    methodName = calledMethods[1].methodName();
    QCOMPARE(QString("unlockFileName"), methodName);
    path = qobject_cast<StringWrapper*>(
        calledMethods[1].params().inParams()[0])->value();
    QCOMPARE(download->filePath(), path);
}

void
TestDownload::testCancelUnlocksPathFromMetadata() {
    QVariantMap metadata;
    QString localPath = "/home/my/local/path";
    metadata["local-path"] = localPath;

    _fileNameMutex->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, metadata, _headers));
    QSignalSpy spy(download.data(),
        SIGNAL(canceled(bool)));  // NOLINT(readability/function)

    download->start();  // change state
    download->startDownload();
    download->cancel();  // change state
    download->cancelDownload();  // method under test

    // assert that the filename was correctly managed
    auto calledMethods = _fileNameMutex->calledMethods();
    QCOMPARE(2, calledMethods.count());
    auto methodName = calledMethods[0].methodName();
    QCOMPARE(QString("lockFileName"), methodName);
    auto path = qobject_cast<StringWrapper*>(
        calledMethods[0].params().inParams()[0])->value();
    QCOMPARE(download->filePath() + ".tmp", path);
    methodName = calledMethods[1].methodName();
    QCOMPARE(QString("unlockFileName"), methodName);
    path = qobject_cast<StringWrapper*>(
        calledMethods[1].params().inParams()[0])->value();
    QCOMPARE(download->filePath() + ".tmp", path);
}

void
TestDownload::testFinishUnlocksPathFromMetadata() {
    QVariantMap metadata;
    QString localPath = "/home/my/local/path";
    metadata["local-path"] = localPath;

    _fileNameMutex->record();
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
	_isConfined, _rootPath, _url, metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(finished(QString)));

    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // emit the finish signal and expect it to be raised
    emit reply->finished();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::FINISH);

    calledMethods = _fileNameMutex->calledMethods();
    QCOMPARE(2, calledMethods.count());
    auto methodName = calledMethods[0].methodName();
    QCOMPARE(QString("lockFileName"), methodName);
    auto path = qobject_cast<StringWrapper*>(
        calledMethods[0].params().inParams()[0])->value();
    QCOMPARE(download->filePath() + ".tmp", path);
    methodName = calledMethods[1].methodName();
    QCOMPARE(QString("unlockFileName"), methodName);
    path = qobject_cast<StringWrapper*>(
        calledMethods[1].params().inParams()[0])->value();
    QCOMPARE(download->filePath() + ".tmp", path);
}

void
TestDownload::testProcessFinishUnlocksPathFromMetadata() {
    _fileNameMutex->record();
    _processFactory->record();
    _reqFactory->record();

    QString command = "cd";
    QVariantMap metadata;
    metadata["post-download-command"] = command;
    QString localPath = "/home/my/local/path";
    metadata["local-path"] = localPath;

    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, metadata, _headers));
    QSignalSpy spy(download.data(), SIGNAL(finished(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // makes the process to be executed
    reply->emitFinished();

    calledMethods = _processFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeProcess* process = reinterpret_cast<FakeProcess*>(
        calledMethods[0].params().outParams()[0]);

    process->emitFinished(0, QProcess::NormalExit);
    QTRY_COMPARE(spy.count(), 1);

    calledMethods = _fileNameMutex->calledMethods();
    QCOMPARE(2, calledMethods.count());
    auto methodName = calledMethods[0].methodName();
    QCOMPARE(QString("lockFileName"), methodName);
    auto path = qobject_cast<StringWrapper*>(
        calledMethods[0].params().inParams()[0])->value();
    QCOMPARE(download->filePath() + ".tmp", path);
    methodName = calledMethods[1].methodName();
    QCOMPARE(QString("unlockFileName"), methodName);
    path = qobject_cast<StringWrapper*>(
        calledMethods[1].params().inParams()[0])->value();
    QCOMPARE(download->filePath() + ".tmp" , path);
}

void
TestDownload::testErrorUnlocksPathFromMetadata() {
    QVariantMap metadata;
    QString localPath = "/home/my/local/path";
    metadata["local-path"] = localPath;

    // fake an error and make sure that unlock is called
    _fileNameMutex->record();
    _reqFactory->record();
    QScopedPointer<FileDownload> download(new FileDownload(_id, _path,
        _isConfined, _rootPath, _url, metadata, _headers));
    QSignalSpy errorSpy(download.data(), SIGNAL(error(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    // set the attrs in the reply so that we do raise two signals
    reply->setAttribute(QNetworkRequest::HttpStatusCodeAttribute, 404);
    reply->setAttribute(QNetworkRequest::HttpReasonPhraseAttribute, "");

    // emit the error and esure that the signals are raised
    reply->emitHttpError(QNetworkReply::ContentAccessDenied);
    QCOMPARE(errorSpy.count(), 1);

    calledMethods = _fileNameMutex->calledMethods();
    QCOMPARE(2, calledMethods.count());
    auto methodName = calledMethods[0].methodName();
    QCOMPARE(QString("lockFileName"), methodName);
    auto path = qobject_cast<StringWrapper*>(
        calledMethods[0].params().inParams()[0])->value();
    QCOMPARE(download->filePath() + ".tmp", path);
    methodName = calledMethods[1].methodName();
    QCOMPARE(QString("unlockFileName"), methodName);
    path = qobject_cast<StringWrapper*>(
        calledMethods[1].params().inParams()[0])->value();
    QCOMPARE(download->filePath() + ".tmp", path);
}
