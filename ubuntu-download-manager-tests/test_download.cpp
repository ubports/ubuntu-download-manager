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
#include <QNetworkRequest>
#include <QSignalSpy>
#include <QSslError>
#include <hash_algorithm.h>
#include "./fake_network_reply.h"
#include "./fake_process.h"
#include "./test_download.h"

TestDownload::TestDownload(QObject* parent)
    : BaseTestCase("TestDownload", parent) {
}

void
TestDownload::init() {
    BaseTestCase::init();

    _id = QUuid::createUuid();
    _isConfined = false;
    _rootPath = testDirectory();
    _path = "random path to dbus";
    _url = QUrl("http://ubuntu.com");
    _algo = "Sha256";
    _networkInfo = new FakeSystemNetworkInfo();
    _reqFactory = new FakeRequestFactory();
    _processFactory = new FakeProcessFactory();
}

void
TestDownload::cleanup() {
    BaseTestCase::cleanup();

    if (_networkInfo)
        delete _networkInfo;
    if (_reqFactory)
        delete _reqFactory;
    if (_processFactory)
        delete _processFactory;
}

void
TestDownload::testNoHashConstructor_data() {
    QTest::addColumn<QUuid>("id");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QUrl>("url");

    QTest::newRow("First row") << QUuid::createUuid() << "/path/to/first/app"
        << QUrl("http://ubuntu.com");
    QTest::newRow("Second row") << QUuid::createUuid() << "/path/to/second/app"
        << QUrl("http://ubuntu.com/juju");
    QTest::newRow("Third row") << QUuid::createUuid() << "/path/to/third/app"
        << QUrl("http://ubuntu.com/tablet");
    QTest::newRow("Last row") << QUuid::createUuid() << "/path/to/last/app"
        << QUrl("http://ubuntu.com/phone");
}

void
TestDownload::testNoHashConstructor() {
    QFETCH(QUuid, id);
    QFETCH(QString, path);
    QFETCH(QUrl, url);

    SingleDownload* download = new SingleDownload(id, path, _isConfined,
        _rootPath, url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));

    // assert that we did set the intial state correctly
    // gets for internal state

    QCOMPARE(download->downloadId(), id);
    QCOMPARE(download->path(), path);
    QCOMPARE(download->url(), url);
    QCOMPARE(download->state(), Download::IDLE);
    QCOMPARE(download->progress(), 0ULL);
    QCOMPARE(download->totalSize(), 0ULL);

    delete download;
}

void
TestDownload::testHashConstructor_data() {
    QTest::addColumn<QUuid>("id");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QString>("hash");
    QTest::addColumn<QString>("algo");

    QTest::newRow("First row") << QUuid::createUuid()
        << "/path/to/first/app" << QUrl("http://ubuntu.com")
        << "my-first-hash" << "md5";
    QTest::newRow("Second row") << QUuid::createUuid()
        << "/path/to/second/app" << QUrl("http://ubuntu.com/juju")
        << "my-second-hash" << "Md5";
    QTest::newRow("Third row") << QUuid::createUuid()
        << "/path/to/third/app" << QUrl("http://ubuntu.com/tablet")
        << "my-third-hash" << "Sha1";
    QTest::newRow("Last row") << QUuid::createUuid()
        << "/path/to/last/app" << QUrl("http://ubuntu.com/phone")
        << "my-last-hash" << "Sha256";
}

void
TestDownload::testHashConstructor() {
    QFETCH(QUuid, id);
    QFETCH(QString, path);
    QFETCH(QUrl, url);
    QFETCH(QString, hash);
    QFETCH(QString, algo);

    SingleDownload* download = new SingleDownload(id, path, _isConfined,
        _rootPath, url, hash, algo, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));

    QCOMPARE(download->downloadId(), id);
    QCOMPARE(download->path(), path);
    QCOMPARE(download->url(), url);
    QCOMPARE(download->hash(), hash);
    QCOMPARE(download->hashAlgorithm(), HashAlgorithm::getHashAlgo(algo));
    QCOMPARE(download->state(), Download::IDLE);
    QCOMPARE(download->progress(), 0ULL);
    QCOMPARE(download->totalSize(), 0ULL);

    delete download;
}

void
TestDownload::testPath_data() {
    // create a number of rows with a diff path to ensure that
    // the accesor does return the correct one
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
    SingleDownload* download = new SingleDownload(_id, path, _isConfined,
        _rootPath, _url, _metadata,
        _headers, QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QCOMPARE(download->path(), path);
}

void
TestDownload::testUrl_data() {
    // create a number of rows with a diff url to ensure that
    // the accesor does return the correct one
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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QCOMPARE(download->url(), url);
    delete download;
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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download , SIGNAL(progress(qulonglong, qulonglong)));

    // start the download so that we do have access to the reply
    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);
    reply->setData(fileData);
    emit reply->downloadProgress(received, total);

    // assert that the total is set and that the signals is emited
    QCOMPARE(download->totalSize(), total);
    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    // assert that the size is not the received but the file size
    QCOMPARE(arguments.at(0).toULongLong(), (qulonglong)fileData.size());
    QCOMPARE(arguments.at(1).toULongLong(), total);
    delete download;
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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download , SIGNAL(progress(qulonglong, qulonglong)));

    // start the download so that we do have access to the reply
    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);
    reply->setData(fileData);
    emit reply->downloadProgress(received, total);

    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    qulonglong size = (qulonglong)fileData.size();
    QCOMPARE(arguments.at(0).toULongLong(), size);
    // must be the same as the progress
    QCOMPARE(arguments.at(1).toULongLong(), size);
    delete download;
}

void
TestDownload::testTotalSize() {
    qulonglong received = 30ULL;
    qulonglong total = 200ULL;

    // assert that the total size is just set once
    // by emitting two signals with diff sizes
    _reqFactory->record();
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download , SIGNAL(progress(qulonglong, qulonglong)));

    // start the download so that we do have access to the reply
    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    emit reply->downloadProgress(received, total);
    emit reply->downloadProgress(received, 2*total);

    QCOMPARE(download->totalSize(), total);
    QCOMPARE(spy.count(), 2);
}

void
TestDownload::testTotalSizeNoProgress() {
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QCOMPARE(0ULL, download->totalSize());
    delete download;
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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    download->setThrottle(speed);

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

    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    download->allowGSMDownload(value);
    QSignalSpy spy(download , SIGNAL(stateChanged()));

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

    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    download->allowGSMDownload(oldValue);
    QSignalSpy spy(download , SIGNAL(stateChanged()));

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

    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
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

    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    download->allowGSMDownload(false);

    QCOMPARE(result, download->canDownload());
    QList<MethodData> calledMethods = _networkInfo->calledMethods();
    QCOMPARE(1, calledMethods.count());
}

void
TestDownload::testCancel() {
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->cancel();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::CANCEL);
    delete download;
}

void
TestDownload::testPause() {
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->pause();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::PAUSE);
    delete download;
}

void
TestDownload::testResume() {
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->resume();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::RESUME);
    delete download;
}

void
TestDownload::testStart() {
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->start();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::START);
    delete download;
}

void
TestDownload::testCancelDownload() {
    // tell the fake nam to record so that we can access the reply

    _reqFactory->record();
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download,
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
    delete download;
}

void
TestDownload::testCancelDownloadNotStarted() {
    _reqFactory->record();
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download,
        SIGNAL(canceled(bool)));  // NOLINT(readability/function)

    download->cancel();  // change state
    download->cancelDownload();  // method under test

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(0, calledMethods.count());
    QCOMPARE(spy.count(), 1);

    // assert file is not present
    QFileInfo info = QFileInfo(download->filePath());
    QVERIFY(!info.exists());
    delete download;
}

void
TestDownload::testPauseDownload() {
    _reqFactory->record();
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download,
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

    delete download;
}

void
TestDownload::testPauseDownloadNotStarted() {
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download,
        SIGNAL(paused(bool)));  // NOLINT(readability/function)

    download->pause();
    download->pauseDownload();

    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(!arguments.at(0).toBool());
    delete download;
}

void
TestDownload::testResumeRunning() {
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
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
}

void
TestDownload::testResumeDownload() {
    _reqFactory->record();
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download , SIGNAL(paused(bool result)));

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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download,
        SIGNAL(started(bool)));  // NOLINT(readability/function)

    download->start();  // change state
    download->startDownload();

    // assert that method was indeed called
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(spy.count(), 1);

    // assert that the files does exist in the system
    QFileInfo info = QFileInfo(download->filePath());
    QVERIFY(info.exists());
    delete download;
}

void
TestDownload::testStartDownloadAlreadyStarted() {
    _reqFactory->record();
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download,
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
    QVERIFY(info.exists());
    delete download;
}

void
TestDownload::testOnSuccessNoHash() {
    _reqFactory->record();
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download , SIGNAL(finished(QString)));

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
}

void
TestDownload::testOnSuccessHashError() {
    _reqFactory->record();
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url,
        "imposible-hash-is-not-hex", _algo, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy errorSpy(download , SIGNAL(error(QString)));

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
    QSignalSpy stateSpy(download , SIGNAL(stateChanged()));
    emit reply->finished();

    // the has is a random string so we should get an error signal
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(download->state(), Download::ERROR);

    delete download;
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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, hash, _algo,
        _metadata, _headers, QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download , SIGNAL(finished(QString)));

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
    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::FINISH);

    delete download;
}

void
TestDownload::testOnHttpError() {
    _reqFactory->record();
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download , SIGNAL(error(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = reinterpret_cast<FakeNetworkReply*>(
        calledMethods[0].params().outParams()[0]);

    QList<QSslError> errors;
    emit reply->sslErrors(errors);
    QCOMPARE(spy.count(), 1);

    delete download;
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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));

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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));

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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download , SIGNAL(paused(bool result)));

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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download , SIGNAL(paused(bool result)));

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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));

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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));

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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));

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

    calledMethods = process->calledMethods();
    StringWrapper* stringWrapper = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().inParams()[0]);
    QString processCommand = stringWrapper->value();
    StringListWrapper* listWrapper = reinterpret_cast<StringListWrapper*>(
        calledMethods[0].params().inParams()[1]);
    QStringList processArgs = listWrapper->value();
    QCOMPARE(processCommand, command);
    QVERIFY(processArgs.contains(download->filePath()));
}

void
TestDownload::testProcessFinishedNoError() {
    QVariantMap metadata;
    QStringList command;
    command << "grep" << "$file" << "-Rn";
    metadata["post-download-command"] = command;

    _processFactory->record();
    _reqFactory->record();
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download , SIGNAL(finished(QString)));

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
}

void
TestDownload::testProcessFinishedWithError() {
    QVariantMap metadata;
    QStringList command;
    command << "grep" << "$file" << "-Rn";
    metadata["post-download-command"] = command;

    _processFactory->record();
    _reqFactory->record();
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download , SIGNAL(error(QString)));

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
    QCOMPARE(spy.count(), 1);
}

void
TestDownload::testProcessFinishedCrash() {
    QVariantMap metadata;
    QStringList command;
    command << "grep" << "$file" << "-Rn";
    metadata["post-download-command"] = command;

    _processFactory->record();
    _reqFactory->record();
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QSignalSpy spy(download , SIGNAL(error(QString)));

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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));

    // write something in the expected file
    QString fileName = download->filePath();
    QFile* file = new QFile(fileName);
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
    // asser that the file does not longer exist in the system
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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));

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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));

    download->start();  // change state
    download->startDownload();

    QSignalSpy stateSpy(download , SIGNAL(stateChanged()));

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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));

    download->start();  // change state
    download->startDownload();

    QSignalSpy stateSpy(download , SIGNAL(stateChanged()));

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

    SingleDownload* download = new SingleDownload(_id, _path, true,
        _rootPath, _url, metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));

    qDebug() << download->filePath();
    QVERIFY(download->filePath() != localPath);
}

void
TestDownload::testLocalPathNotConfined() {
    QVariantMap metadata;
    QString localPath = "/home/my/local/path";
    metadata["local-path"] = localPath;

    SingleDownload* download = new SingleDownload(_id, _path, false,
        _rootPath, _url, metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));

    qDebug() << download->filePath();
    QCOMPARE(download->filePath(), localPath);
}

void
TestDownload::testInvalidUrl() {
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, QUrl(), _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));

    QVERIFY(!download->isValid());
}

void
TestDownload::testValidUrl() {
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));

    QVERIFY(download->isValid());
}

void
TestDownload::testInvalidHashAlgorithm() {
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url,
        "hash", "not-valid-algo", _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
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
    SingleDownload* download = new SingleDownload(_id, _path, _isConfined,
        _rootPath, _url, "hash", algo, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QVERIFY(download->isValid());
}

void
TestDownload::testInvalidFilePresent() {
    // create a file so that we get an error
    QString filePath = testDirectory() + QDir::separator() + "test_file.jpg";
    QFile* file = new QFile(filePath);
    file->open(QIODevice::ReadWrite | QFile::Append);
    file->write("data data data!");
    file->close();

    QVariantMap metadata;
    metadata["local-path"] = filePath;

    SingleDownload* download = new SingleDownload(_id, _path, false,
        _rootPath, _url, metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QVERIFY(!download->isValid());
}

void
TestDownload::testValidFileNotPresent() {
    QString filePath = testDirectory() + QDir::separator() + "test_file.jpg";

    QVariantMap metadata;
    metadata["local-path"] = filePath;

    SingleDownload* download = new SingleDownload(_id, _path, false,
        _rootPath, _url, metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));
    QVERIFY(download->isValid());
}

void
TestDownload::testDownloadPresent() {
    // create a download and get the filename to use, then write it
    // and create the same download and assert that the filename is diff

    SingleDownload* download = new SingleDownload(_id, _path, true,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));

    QString filePath = download->filePath();

    QFile* file = new QFile(filePath);
    file->open(QIODevice::ReadWrite | QFile::Append);
    file->write("data data data!");
    file->close();

    download = new SingleDownload(_id, _path, true,
        _rootPath, _url, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_reqFactory),
        QSharedPointer<ProcessFactory>(_processFactory));

    QVERIFY(filePath != download->filePath());
}
