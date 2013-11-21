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

#include <QSignalSpy>
#include <downloads/factory.h>
#include <downloads/download_struct.h>
#include <system/uuid_utils.h>
#include "fakes/fake_process_factory.h"
#include "fakes/fake_system_network_info.h"
#include "test_download_manager.h"

TestDownloadManager::TestDownloadManager(QObject *parent)
    : BaseTestCase("TestDownloadManager", parent) {
}

void
TestDownloadManager::init() {
    BaseTestCase::init();
    _app = new FakeApplication();
    _appPointer = QSharedPointer<Application>(_app);
    _conn = QSharedPointer<FakeDBusConnection>(new FakeDBusConnection());
    _networkInfo = QSharedPointer<FakeSystemNetworkInfo>(new FakeSystemNetworkInfo());
    _q = new FakeDownloadQueue(QSharedPointer<SystemNetworkInfo>(_networkInfo));
    _uuidFactory = QSharedPointer<FakeUuidFactory>(new FakeUuidFactory());
    _apparmor = new FakeAppArmor(_uuidFactory);
    _requestFactory = new FakeRequestFactory();
    _downloadFactory = new FakeDownloadFactory(
        QSharedPointer<AppArmor>(_apparmor),
        _networkInfo, QSharedPointer<RequestFactory>(_requestFactory),
        QSharedPointer<ProcessFactory>(new FakeProcessFactory()));
    _man = new Manager(_appPointer, _conn, _networkInfo.data(),
        _downloadFactory, _q);
}

void
TestDownloadManager::cleanup() {
    BaseTestCase::cleanup();

    delete _man;
}

QCryptographicHash::Algorithm
TestDownloadManager::algoFromString(const QString& data) {
    QString algoLower = data.toLower();
    if (algoLower == "md5")
        return QCryptographicHash::Md5;
    else if (algoLower == "sha1")
        return QCryptographicHash::Sha1;
    else if (algoLower == "sha224")
        return QCryptographicHash::Sha224;
    else if (algoLower == "sha256")
        return QCryptographicHash::Sha256;
    else if (algoLower == "sha384")
        return QCryptographicHash::Sha384;
    else if (algoLower == "sha512")
        return QCryptographicHash::Sha512;
    return QCryptographicHash::Md5;
}

void
TestDownloadManager::testCreateDownload_data() {
    QTest::addColumn<QString>("url");
    QTest::addColumn<QVariantMap>("metadata");
    QTest::addColumn<StringMap>("headers");

    QVariantMap firstMetadata, secondMetadata, thirdMetadata;
    StringMap firstHeaders, secondHeaders, thirdHeaders;

    firstMetadata["command"] = "cp test.com";
    firstMetadata["download-type"] = "click";

    firstHeaders["header1"] = "headerdata";
    firstHeaders["header2"] = "header2 data";

    QTest::newRow("First row") << "http://ubuntu.com"
        << firstMetadata << firstHeaders;

    secondMetadata["name"] = "paul";
    secondMetadata["download-type"] = "update";

    secondHeaders["Auth"] = "auth header";
    secondHeaders["test"] = "header2 data";

    QTest::newRow("Second row") << "http://ubuntu.com/phone"
        << secondMetadata << secondHeaders;

    thirdMetadata["command"] = "sudo update";
    thirdMetadata["download-type"] = "update";

    thirdHeaders["header1"] = "headerdata";
    thirdHeaders["header2"] = "header2 data";

    QTest::newRow("Last row") << "http://ubuntu.com/phablet"
        << thirdMetadata << thirdHeaders;
}

void
TestDownloadManager::testCreateDownload() {
    QFETCH(QString, url);
    QFETCH(QVariantMap, metadata);
    QFETCH(StringMap, headers);
    _q->record();
    _conn->record();

    // assert that the download is created with the corret info and that
    // we do connect the object to the dbus session
    QSignalSpy spy(_man, SIGNAL(downloadCreated(QDBusObjectPath)));
    DownloadStruct downStruct = DownloadStruct(url, metadata, headers);
    _man->createDownload(downStruct);

    QCOMPARE(spy.count(), 1);

    // grab the created download and assert that
    // it was created correctly with the data
    QList<MethodData> calledMethods = _q->calledMethods();
    QCOMPARE(1, calledMethods.count());

    FileDownload* download = reinterpret_cast<FileDownload*>(
        calledMethods[0].params().inParams()[0]);
    QCOMPARE(UuidUtils::getDBusString(_uuidFactory->data()),
        download->downloadId());
    QCOMPARE(QUrl(url), download->url());

    QVariantMap downloadMetadata = download->metadata();

    foreach(const QString& key, metadata.keys()) {
        QVERIFY(downloadMetadata.contains(key));
        QCOMPARE(metadata[key], downloadMetadata[key]);
    }

    QMap<QString, QString> downloadHeaders = download->headers();

    foreach(const QString& key, headers.keys()) {
        QVERIFY(downloadHeaders.contains(key));
        QCOMPARE(headers[key], downloadHeaders[key]);
    }

    calledMethods = _conn->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("registerObject"), calledMethods[0].methodName());
}

void
TestDownloadManager::testCreateDownloadWithHash_data() {
    QTest::addColumn<QString>("url");
    QTest::addColumn<QString>("algo");
    QTest::addColumn<QString>("hash");
    QTest::addColumn<QVariantMap>("metadata");
    QTest::addColumn<StringMap>("headers");

    QVariantMap firstMetadata, secondMetadata, thirdMetadata;
    StringMap firstHeaders, secondHeaders, thirdHeaders;

    firstMetadata["command"] = "cp test.com";
    firstMetadata["download-type"] = "click";

    firstHeaders["header1"] = "headerdata";
    firstHeaders["header2"] = "header2 data";

    QTest::newRow("First row") << "http://ubuntu.com" << "md5"
        << "my-first-hash" << firstMetadata << firstHeaders;

    secondMetadata["name"] = "paul";
    secondMetadata["download-type"] = "update";

    secondHeaders["Auth"] = "auth header";
    secondHeaders["test"] = "header2 data";

    QTest::newRow("Second row") << "http://ubuntu.com/phone" << "sha1"
        << "my second hash" << secondMetadata << secondHeaders;

    thirdMetadata["command"] = "sudo update";
    thirdMetadata["download-type"] = "update";

    thirdHeaders["header1"] = "headerdata";
    thirdHeaders["header2"] = "header2 data";

    QTest::newRow("Last row") << "http://ubuntu.com/phablet" << "sha256"
        << "my-third-hash" << thirdMetadata << thirdHeaders;
}

void
TestDownloadManager::testCreateDownloadWithHash() {
    QFETCH(QString, url);
    QFETCH(QString, algo);
    QFETCH(QString, hash);
    QFETCH(QVariantMap, metadata);
    QFETCH(StringMap, headers);
    _q->record();
    _conn->record();

    // assert that the download is created with the corret info and that
    // we do connect the object to the dbus session
    QSignalSpy spy(_man, SIGNAL(downloadCreated(QDBusObjectPath)));
    DownloadStruct downStruct = DownloadStruct(url, hash, algo, metadata,
        headers);
    _man->createDownload(downStruct);

    QCOMPARE(spy.count(), 1);

    // grab the created download and assert that it was created
    // correctly with the data
    QList<MethodData> calledMethods = _q->calledMethods();
    QCOMPARE(1, calledMethods.count());

    FileDownload* download = reinterpret_cast<FileDownload*>(
        calledMethods[0].params().inParams()[0]);
    QCOMPARE(UuidUtils::getDBusString(_uuidFactory->data()),
        download->downloadId());
    QCOMPARE(QUrl(url), download->url());

    QVariantMap downloadMetadata = download->metadata();

    QCOMPARE(hash, download->hash());
    QCOMPARE(algoFromString(algo), download->hashAlgorithm());

    foreach(const QString& key, metadata.keys()) {
        QVERIFY(downloadMetadata.contains(key));
        QCOMPARE(metadata[key], downloadMetadata[key]);
    }

    StringMap downloadHeaders = download->headers();

    foreach(const QString& key, headers.keys()) {
        QVERIFY(downloadHeaders.contains(key));
        QCOMPARE(headers[key], downloadHeaders[key]);
    }

    calledMethods = _conn->calledMethods();
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("registerObject"), calledMethods[0].methodName());
}

void
TestDownloadManager::testGetAllDownloads() {
    // add a number of downloads and assert that all the paths are returned
    _q->record();
    _conn->record();

    // do not use the fake uuid factory, else we only get one object path
    _apparmor = new FakeAppArmor(QSharedPointer<UuidFactory>(
        new UuidFactory()));
    _downloadFactory = new FakeDownloadFactory(
        QSharedPointer<AppArmor>(_apparmor),
        QSharedPointer<SystemNetworkInfo>(new FakeSystemNetworkInfo()),
        QSharedPointer<RequestFactory>(new FakeRequestFactory()),
        QSharedPointer<ProcessFactory>(new FakeProcessFactory()));
    _man = new Manager(_appPointer, _conn, _networkInfo.data(),
        _downloadFactory, _q);

    QSignalSpy spy(_man, SIGNAL(downloadCreated(QDBusObjectPath)));

    QString firstUrl("http://www.ubuntu.com"),
            secondUrl("http://www.ubuntu.com/phone"),
            thirdUrl("http://www");
    QVariantMap firstMetadata, secondMetadata, thirdMetadata;
    StringMap firstHeaders, secondHeaders, thirdHeaders;

    _man->createDownload(
        DownloadStruct(firstUrl, firstMetadata, firstHeaders));
    _man->createDownload(
        DownloadStruct(secondUrl, secondMetadata, secondHeaders));
    _man->createDownload(
        DownloadStruct(thirdUrl, thirdMetadata, thirdHeaders));

    QCOMPARE(spy.count(), 3);

    // get the diff create downloads and theri paths so
    // that we can assert that they are returned
    QList<MethodData> calledMethods = _q->calledMethods();
    QCOMPARE(3, calledMethods.count());
    QList<QString> paths;
    for (int index = 0; index < calledMethods.count(); index++) {
        FileDownload* download = reinterpret_cast<FileDownload*>(
            calledMethods[index].params().inParams()[0]);
        paths << download->path();
    }

    QList<QDBusObjectPath> allDownloads = _man->getAllDownloads();
    QCOMPARE(paths.count(), allDownloads.count());
}

void
TestDownloadManager::testAllDownloadsWithMetadata() {
    // add a number of downloads and assert that all the paths are returned
    _q->record();
    _conn->record();

    // do not use the fake uuid factory, else we only get one object path
    _apparmor = new FakeAppArmor(QSharedPointer<UuidFactory>(
        new UuidFactory()));
    _downloadFactory = new FakeDownloadFactory(
        QSharedPointer<AppArmor>(_apparmor),
        QSharedPointer<SystemNetworkInfo>(new FakeSystemNetworkInfo()),
        QSharedPointer<RequestFactory>(new FakeRequestFactory()),
        QSharedPointer<ProcessFactory>(new FakeProcessFactory()));
    _man = new Manager(_appPointer, _conn, _networkInfo.data(),
        _downloadFactory, _q);

    QSignalSpy spy(_man, SIGNAL(downloadCreated(QDBusObjectPath)));

    QString firstUrl("http://www.ubuntu.com"),
            secondUrl("http://www.ubuntu.com/phone"),
            thirdUrl("http://www");
    QVariantMap firstMetadata, secondMetadata, thirdMetadata;
    StringMap firstHeaders, secondHeaders, thirdHeaders;

    firstMetadata["type"] = "first";
    secondMetadata["type"] = "second";
    thirdMetadata["type"] = "first";

    _man->createDownload(
        DownloadStruct(firstUrl, firstMetadata, firstHeaders));
    _man->createDownload(
        DownloadStruct(secondUrl, secondMetadata, secondHeaders));
    _man->createDownload(
        DownloadStruct(thirdUrl, thirdMetadata, thirdHeaders));

    QCOMPARE(spy.count(), 3);

    // get the diff create downloads and theri paths so that we
    // can assert that they are returned
    QList<MethodData> calledMethods = _q->calledMethods();
    QCOMPARE(3, calledMethods.count());
    QList<QString> downloads;
    for (int index = 0; index < calledMethods.count(); index++) {
        FileDownload* download = reinterpret_cast<FileDownload*>(
            calledMethods[index].params().inParams()[0]);
        downloads << download->path();
    }

    QList<QDBusObjectPath> filtered = _man->getAllDownloadsWithMetadata(
        "type", "first");

    QCOMPARE(2, filtered.count());
}

void
TestDownloadManager::testSetThrottleNotDownloads_data() {
    QTest::addColumn<qulonglong>("speed");

    QTest::newRow("First row") << 200ULL;
    QTest::newRow("Second row") << 1212ULL;
    QTest::newRow("Third row") << 998ULL;
    QTest::newRow("Last row") << 60ULL;
}

void
TestDownloadManager::testSetThrottleNotDownloads() {
    QFETCH(qulonglong, speed);
    _man->setDefaultThrottle(speed);
    QCOMPARE(_man->defaultThrottle(), speed);
}

void
TestDownloadManager::testSetThrottleWithDownloads_data() {
    QTest::addColumn<qulonglong>("speed");

    QTest::newRow("First row") << 200ULL;
    QTest::newRow("Second row") << 1212ULL;
    QTest::newRow("Third row") << 998ULL;
    QTest::newRow("Last row") << 60ULL;
}

void
TestDownloadManager::testSetThrottleWithDownloads() {
    QFETCH(qulonglong, speed);

    // do not use the fake uuid factory, else we only get one object path
    _apparmor = new FakeAppArmor(QSharedPointer<UuidFactory>(
        new UuidFactory()));
    _downloadFactory = new FakeDownloadFactory(
        QSharedPointer<AppArmor>(_apparmor),
        QSharedPointer<SystemNetworkInfo>(new FakeSystemNetworkInfo()),
        QSharedPointer<RequestFactory>(new FakeRequestFactory()),
        QSharedPointer<ProcessFactory>(new FakeProcessFactory()));
    _man = new Manager(_appPointer, _conn, _networkInfo.data(),
        _downloadFactory, _q);

    QString firstUrl("http://www.ubuntu.com"),
            secondUrl("http://www.ubuntu.com/phone"),
            thirdUrl("http://www");
    QVariantMap firstMetadata, secondMetadata, thirdMetadata;
    StringMap firstHeaders, secondHeaders, thirdHeaders;

    firstMetadata["type"] = "first";
    secondMetadata["type"] = "second";
    thirdMetadata["type"] = "first";

    _man->createDownload(
        DownloadStruct(firstUrl, firstMetadata, firstHeaders));
    _man->createDownload(
        DownloadStruct(secondUrl, secondMetadata, secondHeaders));
    _man->createDownload(
        DownloadStruct(thirdUrl, thirdMetadata, thirdHeaders));

    _man->setDefaultThrottle(speed);

    QList<MethodData> calledMethods = _q->calledMethods();
    for (int index = 0; index < calledMethods.count(); index++) {
        FileDownload* download = reinterpret_cast<FileDownload*>(
            calledMethods[index].params().inParams()[0]);
        QCOMPARE(download->throttle(), speed);
    }
}

void
TestDownloadManager::testSizeChangedEmittedOnAddition_data() {
    QTest::addColumn<int>("size");

    QTest::newRow("First row") << 4;
    QTest::newRow("Second row") << 5;
    QTest::newRow("Third row") << 0;
    QTest::newRow("Last row") << 34;
}

void
TestDownloadManager::testSizeChangedEmittedOnAddition() {
    QFETCH(int, size);
    QSignalSpy spy(_man,
        SIGNAL(sizeChanged(int)));  // NOLINT(readability/function)
    _q->setSize(size);
    _q->emitDownloadAdded("");

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), size);
}

void
TestDownloadManager::testSizeChangedEmittedOnRemoval_data() {
    QTest::addColumn<int>("size");

    QTest::newRow("First row") << 4;
    QTest::newRow("Second row") << 5;
    QTest::newRow("Third row") << 0;
    QTest::newRow("Last row") << 34;
}

void
TestDownloadManager::testSizeChangedEmittedOnRemoval() {
    QFETCH(int, size);
    QSignalSpy spy(_man,
        SIGNAL(sizeChanged(int)));  // NOLINT(readability/function)
    _q->setSize(size);
    _q->emitDownloadRemoved("");

    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), size);
}

void
TestDownloadManager::testSetSelfSignedCerts() {
    // assert that the factory does get the certs
    _requestFactory->record();
    QList<QSslCertificate> certs;
    _man->setAcceptedCertificates(certs);

    QList<MethodData> calledMethods = _requestFactory->calledMethods();
    qDebug() << calledMethods;
    QCOMPARE(1, calledMethods.count());
    QCOMPARE(QString("setAcceptedCertificates"), calledMethods[0].methodName());
}

void
TestDownloadManager::testStoppable() {
    _app->record();
    _man = new Manager(
        _appPointer,
        qSharedPointerCast<DBusConnection>(_conn),
        _networkInfo.data(), _downloadFactory, _q, true);
    _man->exit();
    QList<MethodData> calledMethods = _app->calledMethods();
    QCOMPARE(1, calledMethods.count());
}

void
TestDownloadManager::testNotStoppable() {
    _app->record();
    _man = new Manager(
        _appPointer,
        qSharedPointerCast<DBusConnection>(_conn),
        _networkInfo.data(), _downloadFactory, _q, false);
    _man->exit();
    QList<MethodData> calledMethods = _app->calledMethods();
    QCOMPARE(0, calledMethods.count());
}
