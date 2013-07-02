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
    _appId = "app-unique-id";
    _appName = "my super app";
    _path = "random path to dbus";
    _url = QUrl("http://ubuntu.com");
    _algo = QCryptographicHash::Sha256;
    _reqFactory = new FakeRequestFactory();
}

void TestDownload::cleanup()
{
    if (_reqFactory)
        delete _reqFactory;

    // try to remove the test dir
    removeDir(_testDir.absolutePath());
    unsetenv("XDG_DATA_HOME");
}

void TestDownload::testNoHashConstructor_data()
{
    QTest::addColumn<QString>("appId");
    QTest::addColumn<QString>("appName");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QUrl>("url");

    QTest::newRow("First row") << "my-first-app-id" << "my-first-app-name"
        << "/path/to/first/app" << QUrl("http://ubuntu.com");
    QTest::newRow("Second row") << "my-second-app-id" << "my-second-app-name"
        << "/path/to/second/app" << QUrl("http://ubuntu.com/juju");
    QTest::newRow("Third row") << "my-third-app-id" << "my-third-app-name"
        << "/path/to/third/app" << QUrl("http://ubuntu.com/tablet");
    QTest::newRow("Last row") << "my-last-app-id" << "my-last-app-name"
        << "/path/to/last/app" << QUrl("http://ubuntu.com/phone");
}

void TestDownload::testNoHashConstructor()
{
    QFETCH(QString, appId);
    QFETCH(QString, appName);
    QFETCH(QString, path);
    QFETCH(QUrl, url);

    Download* download = new Download(appId, appName, path, url, _reqFactory);

    // assert that we did set the intial state correctly
    // gets for internal state

    QCOMPARE(download->applicationId(), appId);
    QCOMPARE(download->applicationName(), appName);
    QCOMPARE(download->path(), path);
    QCOMPARE(download->url(), url);
    QCOMPARE(download->state(), Download::IDLE);
    QCOMPARE(download->progress(), 0u);
    QCOMPARE(download->totalSize(), 0u);

    delete download;
}

void TestDownload::testHashConstructor_data()
{
    QTest::addColumn<QString>("appId");
    QTest::addColumn<QString>("appName");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QString>("hash");
    QTest::addColumn<int>("algo");

    QTest::newRow("First row") << "my-first-app-id" << "my-first-app-name"
        << "/path/to/first/app" << QUrl("http://ubuntu.com") << "my-first-hash" << (int) QCryptographicHash::Md4;
    QTest::newRow("Second row") << "my-second-app-id" << "my-second-app-name"
        << "/path/to/second/app" << QUrl("http://ubuntu.com/juju") << "my-second-hash" << (int) QCryptographicHash::Md5;
    QTest::newRow("Third row") << "my-third-app-id" << "my-third-app-name"
        << "/path/to/third/app" << QUrl("http://ubuntu.com/tablet") << "my-third-hash" << (int) QCryptographicHash::Sha1;
    QTest::newRow("Last row") << "my-last-app-id" << "my-last-app-name"
        << "/path/to/last/app" << QUrl("http://ubuntu.com/phone") << "my-last-hash" << (int) QCryptographicHash::Sha256;
}

void TestDownload::testHashConstructor()
{
    QFETCH(QString, appId);
    QFETCH(QString, appName);
    QFETCH(QString, path);
    QFETCH(QUrl, url);
    QFETCH(QString, hash);
    QFETCH(int, algo);

    Download* download = new Download(appId, appName, path, url, hash, (QCryptographicHash::Algorithm)algo, _reqFactory);

    QCOMPARE(download->applicationId(), appId);
    QCOMPARE(download->applicationName(), appName);
    QCOMPARE(download->path(), path);
    QCOMPARE(download->url(), url);
    QCOMPARE(download->hash(), hash);
    QCOMPARE((int)download->hashAlgorithm(), algo);
    QCOMPARE(download->state(), Download::IDLE);
    QCOMPARE(download->progress(), 0u);
    QCOMPARE(download->totalSize(), 0u);

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
    Download* download = new Download(_appId, _appName, path, _url, _reqFactory);
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
    Download* download = new Download(_appId, _appName, _path, url, _reqFactory);
    QCOMPARE(download->url(), url);
    delete download;
}

void TestDownload::testApplicationId_data()
{
    // create a number of rows with a diff app id to ensure that
    // the accessor does return the correct one
    QTest::addColumn<QString>("appId");
    QTest::newRow("First row") << "my-first-app-id";
    QTest::newRow("Second row") << "my-second-app-id";
    QTest::newRow("Third row") << "my-third-app-id";
    QTest::newRow("Last row") << "my-last-app-id";
}

void TestDownload::testApplicationId()
{
    QFETCH(QString, appId);
    Download* download = new Download(appId, _appName, _path, _url, _reqFactory);
    QCOMPARE(download->applicationId(), appId);
    delete download;
}

void TestDownload::testApplicationName_data()
{
    // create a number of rows with a diff app name to ensure that
    // the accessor does return the correct one
    QTest::addColumn<QString>("appName");
    QTest::newRow("First row") << "my-first-app-name";
    QTest::newRow("Second row") << "my-second-app-name";
    QTest::newRow("Third row") << "my-third-app-name";
    QTest::newRow("Last row") << "my-last-app-name";
}

void TestDownload::testApplicationName()
{
    QFETCH(QString, appName);
    Download* download = new Download(_appId, appName, _path, _url, _reqFactory);
    QCOMPARE(download->applicationName(), appName);
    delete download;
}

void TestDownload::testMetadata_data()
{
    QTest::addColumn<QString>("appId");
    QTest::addColumn<QString>("appName");
    QTest::newRow("First row") << "my-first-app-name" << "my-first-app-name";
    QTest::newRow("Second row") << "my-first-app-name" << "my-first-app-name";
    QTest::newRow("Third row") << "my-first-app-name" << "my-first-app-name";
    QTest::newRow("Last row") << "my-first-app-name" << "my-first-app-name";

}

void TestDownload::testMetadata()
{
    QFETCH(QString, appId);
    QFETCH(QString, appName);

    Download* download = new Download(appId, appName, _path, _url, _reqFactory);
    QVariantMap metadata = download->metadata();

    // assert that keys are present and data is correct
    QVERIFY(metadata.contains("id"));
    QCOMPARE(metadata["id"].toString(), appId);
    QVERIFY(metadata.contains("name"));
    QCOMPARE(metadata["name"].toString(), appName);
    QVERIFY(metadata.contains("size"));
    QVERIFY(metadata.contains("progress"));
}

void TestDownload::testProgress_data()
{
    QTest::addColumn<QByteArray>("fileData");
    QTest::addColumn<uint>("received");
    QTest::addColumn<uint>("total");

    QTest::newRow("First row") << QByteArray(0, 'f') << 67u << 200u;
    QTest::newRow("Second row") << QByteArray(200, 's') << 45u << 12000u;
    QTest::newRow("Third row") << QByteArray(300, 't') << 2u << 2345u;
    QTest::newRow("Last row") << QByteArray(400, 'l') << 3434u << 2323u;
}

void TestDownload::testProgress()
{
    QFETCH(QByteArray, fileData);
    QFETCH(uint, received);
    QFETCH(uint, total);

    _reqFactory->record();
    Download* download = new Download(_appId, _appName, _path, _url, _reqFactory);
    QSignalSpy spy(download , SIGNAL(progress(uint, uint)));

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
    QCOMPARE(arguments.at(0).toUInt(), (uint)fileData.size());  // assert that the size is not the received but the file size
    QCOMPARE(arguments.at(1).toUInt(), total);
    delete download;
}

void TestDownload::testTotalSize()
{
    uint received = 30u;
    uint total = 200u;

    // assert that the total size is just set once by emitting two signals with diff sizes
    _reqFactory->record();
    Download* download = new Download(_appId, _appName, _path, _url, _reqFactory);
    QSignalSpy spy(download , SIGNAL(progress(uint, uint)));

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
    Download* download = new Download(_appId, _appName, _path, _url, _reqFactory);
    QCOMPARE(0u, download->totalSize());
    delete download;
}

void TestDownload::testCancel()
{
    Download* download = new Download(_appId, _appName, _path, _url, _reqFactory);
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->cancel();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::CANCELED);
    delete download;
}

void TestDownload::testPause()
{
    Download* download = new Download(_appId, _appName, _path, _url, _reqFactory);
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->pause();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::PAUSED);
    delete download;
}

void TestDownload::testResume()
{
    Download* download = new Download(_appId, _appName, _path, _url, _reqFactory);
    QSignalSpy spy(download , SIGNAL(stateChanged()));
    download->resume();

    QCOMPARE(spy.count(), 1);
    QCOMPARE(download->state(), Download::RESUMED);
    delete download;
}

void TestDownload::testStart()
{
    Download* download = new Download(_appId, _appName, _path, _url, _reqFactory);
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
    Download* download = new Download(_appId, _appName, _path, _url, _reqFactory);
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
    QCOMPARE(1, calledMethods.count());

    QCOMPARE(QString("abort"), calledMethods[0].methodName());
    QCOMPARE(spy.count(), 1);

    // assert that the files do not exist in the system
    QFileInfo info = QFileInfo(download->filePath());
    QVERIFY(!info.exists());
    delete download;

}

void TestDownload::testCancelDownloadNotStarted()
{
    _reqFactory->record();
    Download* download = new Download(_appId, _appName, _path, _url, _reqFactory);
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
    Download* download = new Download(_appId, _appName, _path, _url, _reqFactory);
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
    QCOMPARE(2, calledMethods.count());  // abort + readAll

    QCOMPARE(QString("abort"), calledMethods[0].methodName());
    QCOMPARE(QString("readAll"), calledMethods[1].methodName());

    // assert current size is correct (progress)
    QCOMPARE(download->progress(), (uint)reply->data().size());

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toBool());

    delete download;
}

void TestDownload::testPauseDownloadNotStarted()
{
    Download* download = new Download(_appId, _appName, _path, _url, _reqFactory);
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
    Download* download = new Download(_appId, _appName, _path, _url, _reqFactory);
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
    Download* download = new Download(_appId, _appName, _path, _url, _reqFactory);
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
    Download* download = new Download(_appId, _appName, _path, _url, _reqFactory);
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
    Download* download = new Download(_appId, _appName, _path, _url, _reqFactory);
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
    Download* download = new Download(_appId, _appName, _path, _url, _reqFactory);
    QSignalSpy spy(download , SIGNAL(finished()));

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
    Download* download = new Download(_appId, _appName, _path, _url, "imposible-hash-is-not-hex", _algo, _reqFactory);
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
    Download* download = new Download(_appId, _appName, _path, _url, hash, _algo, _reqFactory);
    QSignalSpy spy(download , SIGNAL(finished()));

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
    Download* download = new Download(_appId, _appName, _path, _url, _reqFactory);
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

