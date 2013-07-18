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
#include <stdlib.h>
#include <QNetworkRequest>
#include <QSignalSpy>
#include <QSslError>
#include "fake_network_reply.h"
#include "test_download.h"

TestDownload::TestDownload(QObject* parent) :
    QObject(parent)
{
}

bool TestDownload::removeDir(const QString& dirName)
{
    bool result = true;
    QDir dir(dirName);

    QFlags<QDir::Filter> filter =  QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files;
    if (dir.exists(dirName)) {
        foreach(QFileInfo info, dir.entryInfoList(filter, QDir::DirsFirst)) {
            if (info.isDir()) {
            result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}

void TestDownload::init()
{
    // set the xdg path so that we have control over it
    _testDir = QDir("./tests");
    _testDir.makeAbsolute();

    if (!_testDir.exists())
    {
        _testDir.mkpath(_testDir.absolutePath());
    }

    setenv("XDG_DATA_HOME",
        _testDir.absolutePath().toStdString().c_str(), 1);
    _id = QUuid::createUuid();
    _path = "random path to dbus";
    _url = QUrl("http://ubuntu.com");
    _algo = QCryptographicHash::Sha256;
    _networkInfo = new FakeSystemNetworkInfo();
    _reqFactory = new FakeRequestFactory();
}

void TestDownload::cleanup()
{
    if (_networkInfo)
        delete _networkInfo;
    if (_reqFactory)
        delete _reqFactory;

    // try to remove the test dir
    removeDir(_testDir.absolutePath());
    unsetenv("XDG_DATA_HOME");
}

void TestDownload::testNoHashConstructor_data()
{
    QTest::addColumn<QUuid>("id");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QUrl>("url");

    QTest::newRow("First row") << QUuid::createUuid() << "/path/to/first/app" << QUrl("http://ubuntu.com");
    QTest::newRow("Second row") << QUuid::createUuid() << "/path/to/second/app" << QUrl("http://ubuntu.com/juju");
    QTest::newRow("Third row") << QUuid::createUuid() << "/path/to/third/app" << QUrl("http://ubuntu.com/tablet");
    QTest::newRow("Last row") << QUuid::createUuid() << "/path/to/last/app" << QUrl("http://ubuntu.com/phone");
}

void TestDownload::testNoHashConstructor()
{
    QFETCH(QUuid, id);
    QFETCH(QString, path);
    QFETCH(QUrl, url);

    Download* download = new Download(id, path, url, _metadata, _headers, _networkInfo, _reqFactory);

    // assert that we did set the intial state correctly
    // gets for internal state

    QCOMPARE(download->downloadId(), id);
    QCOMPARE(download->path(), path);
    QCOMPARE(download->url(), url);
    QCOMPARE(download->state(), Download::IDLE);
    QCOMPARE(download->progress(), 0LL);
    QCOMPARE(download->totalSize(), 0LL);

    delete download;
}

void TestDownload::testHashConstructor_data()
{
    QTest::addColumn<QUuid>("id");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QString>("hash");
    QTest::addColumn<int>("algo");

    QTest::newRow("First row") << QUuid::createUuid()
        << "/path/to/first/app" << QUrl("http://ubuntu.com") << "my-first-hash" << (int) QCryptographicHash::Md4;
    QTest::newRow("Second row") << QUuid::createUuid()
        << "/path/to/second/app" << QUrl("http://ubuntu.com/juju") << "my-second-hash" << (int) QCryptographicHash::Md5;
    QTest::newRow("Third row") << QUuid::createUuid()
        << "/path/to/third/app" << QUrl("http://ubuntu.com/tablet") << "my-third-hash" << (int) QCryptographicHash::Sha1;
    QTest::newRow("Last row") << QUuid::createUuid()
        << "/path/to/last/app" << QUrl("http://ubuntu.com/phone") << "my-last-hash" << (int) QCryptographicHash::Sha256;
}

void TestDownload::testHashConstructor()
{
    QFETCH(QUuid, id);
    QFETCH(QString, path);
    QFETCH(QUrl, url);
    QFETCH(QString, hash);
    QFETCH(int, algo);

    Download* download = new Download(id, path, url, hash, (QCryptographicHash::Algorithm)algo, _metadata, _headers, _networkInfo,
        _reqFactory);

    QCOMPARE(download->downloadId(), id);
    QCOMPARE(download->path(), path);
    QCOMPARE(download->url(), url);
    QCOMPARE(download->hash(), hash);
    QCOMPARE((int)download->hashAlgorithm(), algo);
    QCOMPARE(download->state(), Download::IDLE);
    QCOMPARE(download->progress(), 0LL);
    QCOMPARE(download->totalSize(), 0LL);

    delete download;
}

void TestDownload::testPath_data()
{
    // create a number of rows with a diff path to ensure that
    // the accesor does return the correct one
    QTest::addColumn<QString>("path");
    QTest::newRow("First row") << "/first/random/path";
    QTest::newRow("Second row") << "/second/random/path";
    QTest::newRow("Third row") << "/third/random/path";
    QTest::newRow("Last row") << "/last/random/path";
}

void TestDownload::testPath()
{
    // create an app download and assert that the returned data is correct
    QFETCH(QString, path);
    Download* download = new Download(_id, path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    QCOMPARE(download->path(), path);
}

void TestDownload::testUrl_data()
{
    // create a number of rows with a diff url to ensure that
    // the accesor does return the correct one
    QTest::addColumn<QUrl>("url");
    QTest::newRow("First row") << QUrl("http://ubuntu.com");
    QTest::newRow("Second row") << QUrl("http://one.ubuntu.com");
    QTest::newRow("Third row") << QUrl("http://ubuntu.com/phone");
    QTest::newRow("Last row") << QUrl("http://ubuntu.com/tablet");
}

void TestDownload::testUrl()
{
    // create an app download and assert that the returned data is correct
    QFETCH(QUrl, url);
    Download* download = new Download(_id, _path, url, _metadata, _headers, _networkInfo, _reqFactory);
    QCOMPARE(download->url(), url);
    delete download;
}

void TestDownload::testProgress_data()
{
    QTest::addColumn<QByteArray>("fileData");
    QTest::addColumn<qlonglong>("received");
    QTest::addColumn<qlonglong>("total");

    QTest::newRow("First row") << QByteArray(0, 'f') << 67LL << 200LL;
    QTest::newRow("Second row") << QByteArray(200, 's') << 45LL << 12000LL;
    QTest::newRow("Third row") << QByteArray(300, 't') << 2LL << 2345LL;
    QTest::newRow("Last row") << QByteArray(400, 'l') << 3434LL << 2323LL;
}

void TestDownload::testProgress()
{
    QFETCH(QByteArray, fileData);
    QFETCH(qlonglong, received);
    QFETCH(qlonglong, total);

    _reqFactory->record();
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(progress(qlonglong, qlonglong)));

    // start the download so that we do have access to the reply
    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    FakeNetworkReply* reply = (FakeNetworkReply*) calledMethods[0].params().outParams()[0];
    reply->setData(fileData);
    emit reply->downloadProgress(received, total);

    // assert that the total is set and tha the signals is emited
    QCOMPARE(download->totalSize(), total);
    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toLongLong(), (qlonglong)fileData.size());  // assert that the size is not the received but the file size
    QCOMPARE(arguments.at(1).toLongLong(), total);
    delete download;
}

void TestDownload::testTotalSize()
{
    qlonglong received = 30LL;
    qlonglong total = 200LL;

    // assert that the total size is just set once by emitting two signals with diff sizes
    _reqFactory->record();
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(progress(qlonglong, qlonglong)));

    // start the download so that we do have access to the reply
    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    FakeNetworkReply* reply = (FakeNetworkReply*) calledMethods[0].params().outParams()[0];

    emit reply->downloadProgress(received, total);
    emit reply->downloadProgress(received, 2*total);

    QCOMPARE(download->totalSize(), total);
    QCOMPARE(spy.count(), 2);
}

void TestDownload::testTotalSizeNoProgress()
{
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    QCOMPARE(0LL, download->totalSize());
    delete download;
}

void TestDownload::testSetThrottleNoReply_data()
{
    QTest::addColumn<qlonglong>("speed");

    QTest::newRow("First row") << 200LL;
    QTest::newRow("Second row") << 1212LL;
    QTest::newRow("Third row") << 998LL;
    QTest::newRow("Last row") << 60LL;
}

void TestDownload::testSetThrottleNoReply()
{
    QFETCH(qlonglong, speed);
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    download->setThrottle(speed);
    QCOMPARE(speed, download->throttle());
}

void TestDownload::testSetThrottle_data()
{
    QTest::addColumn<uint>("speed");

    QTest::newRow("First row") << 200u;
    QTest::newRow("Second row") << 1212u;
    QTest::newRow("Third row") << 998u;
    QTest::newRow("Last row") << 60u;
}

void TestDownload::testSetThrottle()
{
    QFETCH(uint, speed);

    _reqFactory->record();
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    download->setThrottle(speed);

    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    FakeNetworkReply* reply = (FakeNetworkReply*) calledMethods[0].params().outParams()[0];
    calledMethods = reply->calledMethods();

    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("setReadBufferSize"), calledMethods[0].methodName());
    QCOMPARE(speed, ((UintWrapper*)calledMethods[0].params().inParams()[0])->value());
}

void TestDownload::testSetGSMDownloadSame_data()
{
    QTest::addColumn<bool>("value");

    QTest::newRow("True") << true;
    QTest::newRow("False") << false;
}

void TestDownload::testSetGSMDownloadSame()
{
    QFETCH(bool, value);

    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    download->allowGSMDownload(value);
    QSignalSpy spy(download , SIGNAL(stateChanged()));

    download->allowGSMDownload(value);
    QCOMPARE(spy.count(), 0);
}

void TestDownload::testSetGSMDownloadDiff_data()
{
    QTest::addColumn<bool>("oldValue");
    QTest::addColumn<bool>("newValue");

    QTest::newRow("True") << true << false;
    QTest::newRow("False") << false << true;
}

void TestDownload::testSetGSMDownloadDiff()
{
    QFETCH(bool, oldValue);
    QFETCH(bool, newValue);

    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    download->allowGSMDownload(oldValue);
    QSignalSpy spy(download , SIGNAL(stateChanged()));

    download->allowGSMDownload(newValue);
    QCOMPARE(spy.count(), 1);
}

void TestDownload::testCanDownloadGSM_data()
{
    QTest::addColumn<QVariant>("mode");

    QVariant gsmMode, cdmaMode, wCdmaMode, wlanMode, ethernetMode, bluetoothMode, wimaxMode, lteMode, tdscdmaMode;

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

void TestDownload::testCanDownloadGSM()
{
    QFETCH(QVariant, mode);
    _networkInfo->setMode(mode.value<QNetworkInfo::NetworkMode>());
    _networkInfo->record();

    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    download->allowGSMDownload(true);
    QVERIFY(download->canDownload());
    QList<MethodData> calledMethods = _networkInfo->calledMethods();
    QCOMPARE(1, calledMethods.count());
}

void TestDownload::testCanDownloadNoGSM_data()
{
    QTest::addColumn<QVariant>("mode");
    QTest::addColumn<bool>("result");

    QVariant unknownMode, gsmMode, cdmaMode, wCdmaMode, wlanMode, ethernetMode, bluetoothMode, wimaxMode, lteMode, tdscdmaMode;

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

void TestDownload::testCanDownloadNoGSM()
{
    QFETCH(QVariant, mode);
    QFETCH(bool, result);
    _networkInfo->setMode(mode.value<QNetworkInfo::NetworkMode>());
    _networkInfo->record();

    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    download->allowGSMDownload(false);

    QCOMPARE(result, download->canDownload());
    QList<MethodData> calledMethods = _networkInfo->calledMethods();
    QCOMPARE(1, calledMethods.count());
}

void TestDownload::testCancel()
{
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->cancel();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::CANCELED);
    delete download;
}

void TestDownload::testPause()
{
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->pause();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::PAUSED);
    delete download;
}

void TestDownload::testResume()
{
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->resume();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::RESUMED);
    delete download;
}

void TestDownload::testStart()
{
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->start();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::STARTED);
    delete download;
}

void TestDownload::testCancelDownload()
{
    // tell the fake nam to record so that we can access the reply

    _reqFactory->record();
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(canceled(bool)));

    download->start();  // change state
    download->startDownload();
    download->cancel(); // change state
    download->cancelDownload();  // method under test

    // assert that method was indeed called
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = (FakeNetworkReply*) calledMethods[0].params().outParams()[0];

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

void TestDownload::testCancelDownloadNotStarted()
{
    _reqFactory->record();
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(canceled(bool)));

    download->cancel(); // change state
    download->cancelDownload();  // method under test

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(0, calledMethods.count());
    QCOMPARE(spy.count(), 1);

    // assert file is not present
    QFileInfo info = QFileInfo(download->filePath());
    QVERIFY(!info.exists());
    delete download;
}

void TestDownload::testPauseDownload()
{
    _reqFactory->record();
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(paused(bool)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = (FakeNetworkReply*) calledMethods[0].params().outParams()[0];
    reply->setData(QByteArray(900, 's'));

    download->pause(); // change state
    download->pauseDownload();  // method under test

    // assert that the reply was aborted and deleted via deleteLater
    calledMethods = reply->calledMethods();
    QCOMPARE(3, calledMethods.count());  // throttel + abort + readAll

    QCOMPARE(QString("abort"), calledMethods[1].methodName());
    QCOMPARE(QString("readAll"), calledMethods[2].methodName());

    // assert current size is correct (progress)
    QCOMPARE(download->progress(), (qlonglong)reply->data().size());

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toBool());

    delete download;
}

void TestDownload::testPauseDownloadNotStarted()
{
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(paused(bool)));

    download->pause();
    download->pauseDownload();

    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(!arguments.at(0).toBool());
    delete download;
}

void TestDownload::testResumeRunning()
{
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(resumed(bool)));

    download->start();
    download->startDownload();
    download->resume();
    download->resumeDownload();

    QCOMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(!arguments.at(0).toBool());
    delete download;
}

void TestDownload::testResumeDownload()
{
    _reqFactory->record();
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(paused(bool)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = (FakeNetworkReply*) calledMethods[0].params().outParams()[0];
    reply->setData(QByteArray(900, 's'));

    download->pause(); // change state
    download->pauseDownload();  // method under test

    // clear the called methods from the reqFactory
    _reqFactory->clear();
    download->resume();
    download->resumeDownload();

    // get the info for the second created request
    calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QNetworkRequest request = ((RequestWrapper*)calledMethods[0].params().inParams()[0])->request();
    // assert that the request has the correct headers set
    QVERIFY(request.hasRawHeader("Range"));
    QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(reply->data().size()) + "-";
    QCOMPARE(rangeHeaderValue, request.rawHeader("Range"));
}

void TestDownload::testStartDownload()
{
    _reqFactory->record();
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(started(bool)));

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

void TestDownload::testStartDownloadAlreadyStarted()
{
    _reqFactory->record();
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(started(bool)));

    download->start();  // change state
    download->startDownload();
    download->startDownload();  // second redundant call under test

    // assert that method was indeed called
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());  // if the reqFactory is called twice we have a bug
    QCOMPARE(spy.count(), 2);  // signal should be raised twice because we called it twice

    // assert that the files does exist in the system
    QFileInfo info = QFileInfo(download->filePath());
    QVERIFY(info.exists());
    delete download;
}

void TestDownload::testOnSuccessNoHash()
{
    _reqFactory->record();
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(finished(QString)));

    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = (FakeNetworkReply*) calledMethods[0].params().outParams()[0];

    // emit the finish signal and expect it to be raised
    emit reply->finished();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::FINISHED);
}

void TestDownload::testOnSuccessHashError()
{
    _reqFactory->record();
    Download* download = new Download(_id, _path, _url, "imposible-hash-is-not-hex", _algo, _metadata, _headers,
        _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(error(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = (FakeNetworkReply*) calledMethods[0].params().outParams()[0];
    reply->setData(QByteArray(200, 'f'));

    download->pause();
    download->pauseDownload();   // trick to write the data in the internal storage

    // clear the called methods from the reqFactory
    _reqFactory->clear();
    download->resume();
    download->resumeDownload();

    calledMethods = _reqFactory->calledMethods();
    reply = (FakeNetworkReply*) calledMethods[0].params().outParams()[0];
    emit reply->finished();

    // the has is a random string so we should get an error signal
    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::FINISHED);

    delete download;
}

void TestDownload::testOnSuccessHash_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QString>("hash");

    QByteArray firstData(0, 'f');
    QByteArray secondData(200, 's');
    QByteArray thirdData(300, 't');
    QByteArray lastData(400, 'l');

    QTest::newRow("First row") << firstData
        << QString(QCryptographicHash::hash(firstData, _algo).toHex());
    QTest::newRow("Second row") << secondData
        << QString(QCryptographicHash::hash(secondData, _algo).toHex());
    QTest::newRow("Third row") << thirdData
        << QString(QCryptographicHash::hash(thirdData, _algo).toHex());
    QTest::newRow("Last row") << lastData
        << QString(QCryptographicHash::hash(lastData, _algo).toHex());
}

void TestDownload::testOnSuccessHash()
{
    QFETCH(QByteArray, data);
    QFETCH(QString, hash);

    _reqFactory->record();
    Download* download = new Download(_id, _path, _url, hash, _algo, _metadata, _headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(finished(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = (FakeNetworkReply*) calledMethods[0].params().outParams()[0];
    reply->setData(data);

    download->pause();
    download->pauseDownload();   // trick to write the data in the internal storage

    // clear the called methods from the reqFactory
    _reqFactory->clear();
    download->resume();
    download->resumeDownload();

    calledMethods = _reqFactory->calledMethods();
    reply = (FakeNetworkReply*) calledMethods[0].params().outParams()[0];
    emit reply->finished();

    // the hash should be correct and we should get the finish signal
    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::FINISHED);

    delete download;
}

void TestDownload::testOnHttpError()
{
    _reqFactory->record();
    Download* download = new Download(_id, _path, _url, _metadata, _headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(error(QString)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = (FakeNetworkReply*) calledMethods[0].params().outParams()[0];

    QList<QSslError> errors;
    emit reply->sslErrors(errors);
    QCOMPARE(spy.count(), 1);

    delete download;
}

void TestDownload::testSetRawHeadersStart_data()
{
    QTest::addColumn<QMap<QString, QString> >("headers");

    // create a number of headers to assert that thy are added in the request
    StringMap first, second, third;

    // add headers to be added except range
    first["Accept"] = "text/plain";
    first["Accept-Charset"] = "utf-8";
    first["Accept-Encoding"] = "gzip, deflate";

    QTest::newRow("First row") << first;

    second["Accept-Language"] = "en-US";
    second["Cache-Control"] = "no-cache";
    second["Connection"] = "keep-alive";

    QTest::newRow("Second row") << second;

    third["Content-Length"] = "348";
    third["User-Agent"] = "Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/21.0";

    QTest::newRow("Third row") << third;
}

void TestDownload::testSetRawHeadersStart()
{
    QFETCH(StringMap, headers);
    _reqFactory->record();
    Download* download = new Download(_id, _path, _url, _metadata, headers, _networkInfo, _reqFactory);

    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QNetworkRequest request = ((RequestWrapper*)calledMethods[0].params().inParams()[0])->request();

    // assert that all headers are present
    foreach(const QString& header, headers.keys())
    {
        QByteArray headerName = header.toUtf8();
        QVERIFY(request.hasRawHeader(headerName));
        QCOMPARE(headers[header].toUtf8(), request.rawHeader(headerName));
    }

}

void TestDownload::testSetRawHeadersWithRangeStart_data()
{
    QTest::addColumn<StringMap>("headers");

    // create a number of headers to assert that thy are added in the request
    StringMap first, second, third;

    // add headers to be added except range
    first["Accept"] = "text/plain";
    first["Accept-Charset"] = "utf-8";
    first["Accept-Encoding"] = "gzip, deflate";
    first["Range"] = "gzip, deflate";

    QTest::newRow("First row") << first;

    second["Accept-Language"] = "en-US";
    second["Cache-Control"] = "no-cache";
    second["Connection"] = "keep-alive";
    second["Range"] = "gzip, deflate";

    QTest::newRow("Second row") << second;

    third["Content-Length"] = "348";
    third["User-Agent"] = "Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/21.0";
    third["Range"] = "gzip, deflate";

    QTest::newRow("Third row") << third;
}

void TestDownload::testSetRawHeadersWithRangeStart()
{
    // similar to the previous test but we want to ensure that range is not set
    QFETCH(StringMap, headers);
    _reqFactory->record();
    Download* download = new Download(_id, _path, _url, _metadata, headers, _networkInfo, _reqFactory);

    download->start();  // change state
    download->startDownload();

    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QNetworkRequest request = ((RequestWrapper*)calledMethods[0].params().inParams()[0])->request();

    // assert that range is not present
    QVERIFY(!request.hasRawHeader("Range"));
}

void TestDownload::testSetRawHeadersResume_data()
{
    // same as with start but we test that all the headers are added + the range one
    QTest::addColumn<StringMap>("headers");

    // create a number of headers to assert that thy are added in the request
    StringMap first, second, third;

    // add headers to be added except range
    first["Accept"] = "text/plain";
    first["Accept-Charset"] = "utf-8";
    first["Accept-Encoding"] = "gzip, deflate";

    QTest::newRow("First row") << first;

    second["Accept-Language"] = "en-US";
    second["Cache-Control"] = "no-cache";
    second["Connection"] = "keep-alive";

    QTest::newRow("Second row") << second;

    third["Content-Length"] = "348";
    third["User-Agent"] = "Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/21.0";

    QTest::newRow("Third row") << third;
}

void TestDownload::testSetRawHeadersResume()
{
    QFETCH(StringMap, headers);

    _reqFactory->record();
    Download* download = new Download(_id, _path, _url, _metadata, headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(paused(bool)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = (FakeNetworkReply*) calledMethods[0].params().outParams()[0];
    reply->setData(QByteArray(900, 's'));

    download->pause(); // change state
    download->pauseDownload();  // method under test

    // clear the called methods from the reqFactory
    _reqFactory->clear();
    download->resume();
    download->resumeDownload();

    // get the info for the second created request
    calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QNetworkRequest request = ((RequestWrapper*)calledMethods[0].params().inParams()[0])->request();

    // assert that the request has the correct range header set
    QVERIFY(request.hasRawHeader("Range"));
    QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(reply->data().size()) + "-";
    QCOMPARE(rangeHeaderValue, request.rawHeader("Range"));

    // assert that it has the rest of the headers
    foreach(const QString& header, headers.keys())
    {
        QByteArray headerName = header.toUtf8();
        QVERIFY(request.hasRawHeader(headerName));
        QCOMPARE(headers[header].toUtf8(), request.rawHeader(headerName));
    }
}

void TestDownload::testSetRawHeadersWithRangeResume_data()
{
    QTest::addColumn<StringMap>("headers");

    // create a number of headers to assert that thy are added in the request
    StringMap first, second, third;

    // add headers to be added except range
    first["Accept"] = "text/plain";
    first["Accept-Charset"] = "utf-8";
    first["Accept-Encoding"] = "gzip, deflate";
    first["Range"] = "gzip, deflate";

    QTest::newRow("First row") << first;

    second["Accept-Language"] = "en-US";
    second["Cache-Control"] = "no-cache";
    second["Connection"] = "keep-alive";
    second["Range"] = "gzip, deflate";

    QTest::newRow("Second row") << second;

    third["Content-Length"] = "348";
    third["User-Agent"] = "Mozilla/5.0 (X11; Linux x86_64; rv:12.0) Gecko/20100101 Firefox/21.0";
    third["Range"] = "gzip, deflate";

    QTest::newRow("Third row") << third;
}

void TestDownload::testSetRawHeadersWithRangeResume()
{
    // same as the previous test but we assert that range is correctly set
    QFETCH(StringMap, headers);

    _reqFactory->record();
    Download* download = new Download(_id, _path, _url, _metadata, headers, _networkInfo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(paused(bool)));

    download->start();  // change state
    download->startDownload();

    // we need to set the data before we pause!!!
    QList<MethodData> calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    FakeNetworkReply* reply = (FakeNetworkReply*) calledMethods[0].params().outParams()[0];
    reply->setData(QByteArray(900, 's'));

    download->pause(); // change state
    download->pauseDownload();  // method under test

    // clear the called methods from the reqFactory
    _reqFactory->clear();
    download->resume();
    download->resumeDownload();

    // get the info for the second created request
    calledMethods = _reqFactory->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QNetworkRequest request = ((RequestWrapper*)calledMethods[0].params().inParams()[0])->request();

    // assert that the request has the correct range header set
    QVERIFY(request.hasRawHeader("Range"));
    QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(reply->data().size()) + "-";
    QCOMPARE(rangeHeaderValue, request.rawHeader("Range"));
}
