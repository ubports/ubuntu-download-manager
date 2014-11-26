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

#include <ubuntu/downloads/factory.h>
#include <ubuntu/download_manager/download_struct.h>
#include <ubuntu/transfers/system/uuid_utils.h>
#include <ubuntu/transfers/system/process_factory.h>
#include <ubuntu/transfers/system/network_session.h>
#include <gmock/gmock.h>
#include "dbus_proxy.h"
#include "download.h"
#include "matchers.h"
#include "pending_reply.h"
#include "test_download_manager.h"

using ::testing::_;
using ::testing::Eq;
using ::testing::ByRef;
using ::testing::Mock;
using ::testing::Return;

void
TestDownloadManager::init() {
    BaseTestCase::init();
    _app = new MockApplication();
    _database = new MockDatabase();
    DownloadsDb::setInstance(_database);
    _conn = new MockDBusConnection();
    _q = new MockDownloadQueue();
    _requestFactory = new MockRequestFactory();
    RequestFactory::setInstance(_requestFactory);
    _factory = new MockDownloadFactory();
    _man = new DownloadManager(_app, _conn, _factory, _q);
    _dbusProxyFactory = new MockDBusProxyFactory();
    DBusProxyFactory::setInstance(_dbusProxyFactory );
}

void
TestDownloadManager::cleanup() {
    BaseTestCase::cleanup();

    NetworkSession::deleteInstance();
    RequestFactory::deleteInstance();
    ProcessFactory::deleteInstance();
    DownloadsDb::deleteInstance();
    DBusProxyFactory::deleteInstance();
    delete _man;
    delete _conn;
    delete _app;
}

void
TestDownloadManager::verifyMocks() {
    QVERIFY(Mock::VerifyAndClearExpectations(_app));
    QVERIFY(Mock::VerifyAndClearExpectations(_database));
    QVERIFY(Mock::VerifyAndClearExpectations(_conn));
    QVERIFY(Mock::VerifyAndClearExpectations(_factory));
    QVERIFY(Mock::VerifyAndClearExpectations(_q));
    QVERIFY(Mock::VerifyAndClearExpectations(_requestFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(_dbusProxyFactory));
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
    QString dbusPath = "/path/to/object";
    QScopedPointer<MockDownload> down(new MockDownload("", "", "", "",
        QUrl(url), metadata, headers));

    // assert that the download is created with the corret info and that
    // we do connect the object to the dbus session
    SignalBarrier spy(_man, SIGNAL(downloadCreated(QDBusObjectPath)));
    DownloadStruct downStruct(url, metadata, headers);

    // set the expectations of the factory since is the one that
    // creates the downloads. The matchers will ensure that the
    // correct value is used.
    EXPECT_CALL(*_factory, createDownload(_, Eq(url), QVariantMapEq(metadata),
        QStringMapEq(headers)))
            .Times(1)
            .WillRepeatedly(Return(down.data()));

    // expected actions to be performed on the download
    EXPECT_CALL(*down.data(), setThrottle(_man->defaultThrottle()))
        .Times(1);

    EXPECT_CALL(*down.data(), path())
        .Times(1)
        .WillRepeatedly(Return(dbusPath));

    // expected actions performed by the db
    EXPECT_CALL(*_database, store(down.data()))
        .Times(1)
        .WillRepeatedly(Return(true));

    // expected calls performed by the q
    EXPECT_CALL(*_q, add(down.data()))
        .Times(1);

    // expected calls performed by the conn
    EXPECT_CALL(*_conn, registerObject(dbusPath, down.data(), _))
        .Times(1)
        .WillRepeatedly(Return(true));

    _man->createDownload(downStruct);

    QVERIFY(spy.ensureSignalEmitted());
    QCOMPARE(spy.count(), 1);

    QVERIFY(Mock::VerifyAndClearExpectations(down.data()));
    verifyMocks();
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
    QString dbusPath = "/path/to/object";
    QScopedPointer<MockDownload> down(new MockDownload("", "", "", "",
        QUrl(url), metadata, headers));

    // assert that the download is created with the corret info and that
    // we do connect the object to the dbus session
    SignalBarrier spy(_man, SIGNAL(downloadCreated(QDBusObjectPath)));
    DownloadStruct downStruct = DownloadStruct(url, hash, algo, metadata,
        headers);

    // set the expectations of the factory since is the one that
    // creates the downloads. The matchers will ensure that the
    // correct value is used.
    EXPECT_CALL(*_factory, createDownload(_, Eq(url), hash, algo,
        QVariantMapEq(metadata), QStringMapEq(headers)))
            .Times(1)
            .WillRepeatedly(Return(down.data()));

    // expected actions to be performed on the download
    EXPECT_CALL(*down.data(), setThrottle(_man->defaultThrottle()))
        .Times(1);

    EXPECT_CALL(*down.data(), allowGSMDownload(_))
        .Times(1);

    EXPECT_CALL(*down.data(), path())
        .Times(1)
        .WillRepeatedly(Return(dbusPath));

    // expected actions performed by the db
    EXPECT_CALL(*_database, store(down.data()))
        .Times(1)
        .WillRepeatedly(Return(true));

    // expected calls performed by the q
    EXPECT_CALL(*_q, add(down.data()))
        .Times(1);

    // expected calls performed by the conn
    EXPECT_CALL(*_conn, registerObject(dbusPath, down.data(), _))
        .Times(1)
        .WillRepeatedly(Return(true));

    _man->createDownload(downStruct);

    QVERIFY(spy.ensureSignalEmitted());
    QCOMPARE(spy.count(), 1);

    QVERIFY(Mock::VerifyAndClearExpectations(down.data()));
    verifyMocks();
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

    // return no downloads
    EXPECT_CALL(*_q, transfers())
        .Times(1)
        .WillRepeatedly(Return(QHash<QString, Transfer*>()));

    _man->setDefaultThrottle(speed);
    QCOMPARE(_man->defaultThrottle(), speed);
    verifyMocks();
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
    QVariantMap metadata;
    QMap<QString, QString> headers;
    QScopedPointer<MockDownload> first(new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), metadata, headers));
    QScopedPointer<MockDownload> second(new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), metadata, headers));
    QScopedPointer<MockDownload> third(new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), metadata, headers));
    QHash<QString, Transfer*> downs;
    downs["/first/object/path"] = first.data();
    downs["/second/object/path"] = second.data();
    downs["/third/object/path"] = third.data();

    // set expectations
    EXPECT_CALL(*_q, transfers())
        .WillRepeatedly(Return(downs));

    foreach(auto key, downs.keys()) {
        auto mock = static_cast<MockDownload*>(downs[key]);
        EXPECT_CALL(*mock, setThrottle(speed))
            .Times(1);
    }

    _man->setDefaultThrottle(speed);

    foreach(auto key, downs.keys()) {
         QVERIFY(Mock::VerifyAndClearExpectations(downs[key]));
    }

    verifyMocks();
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
    SignalBarrier spy(_man,
        SIGNAL(sizeChanged(int)));  // NOLINT(readability/function)

    EXPECT_CALL(*_q, size())
        .Times(1)
        .WillRepeatedly(Return(size));

    _q->transferAdded("");

    QVERIFY(spy.ensureSignalEmitted());
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), size);
    verifyMocks();
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
    SignalBarrier spy(_man,
        SIGNAL(sizeChanged(int)));  // NOLINT(readability/function)

    EXPECT_CALL(*_q, size())
        .Times(1)
        .WillRepeatedly(Return(size));

    _q->transferRemoved("");

    QVERIFY(spy.ensureSignalEmitted());
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), size);
    verifyMocks();
}

void
TestDownloadManager::testSetSelfSignedCerts() {
    // assert that the factory does get the certs
    QList<QSslCertificate> certs;

    EXPECT_CALL(*_factory, setAcceptedCertificates(_))
        .Times(1);
    _man->setAcceptedCertificates(certs);

    verifyMocks();
}

void
TestDownloadManager::testStoppable() {
    auto q = new MockDownloadQueue();
    auto factory = new MockDownloadFactory();

    QScopedPointer<DownloadManager> man(
        new DownloadManager(_app, _conn, factory, q, true));

    EXPECT_CALL(*_app, exit(0))
        .Times(1);

    man->exit();
    verifyMocks();
}

void
TestDownloadManager::testNotStoppable() {
    auto q = new MockDownloadQueue();
    auto factory = new MockDownloadFactory();

    QScopedPointer<DownloadManager> man(
        new DownloadManager(_app, _conn, factory, q, false));

    EXPECT_CALL(*_app, exit(0))
        .Times(0);  // never exit!

    man->exit();
    verifyMocks();
}

void
TestDownloadManager::testGetAllDownloadsUnconfined() {
    QString expectedAppId = "unconfined";
    auto dbusProxy = new MockDBusProxy();
    auto reply = new MockPendingReply<QString>();

    EXPECT_CALL(*_dbusProxyFactory, createDBusProxy(_))
        .Times(1)
        .WillOnce(Return(dbusProxy));

    EXPECT_CALL(*dbusProxy, GetConnectionAppArmorSecurityContext(_))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, waitForFinished())
        .Times(1);

    EXPECT_CALL(*reply, isError())
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*reply, value())
        .Times(1)
        .WillOnce(Return(expectedAppId));

    QStringList expectedPaths;
    expectedPaths.append("/first/path/object");
    expectedPaths.append("/second/path/object");
    expectedPaths.append("/third/path/object");

    EXPECT_CALL(*_q, paths())
        .Times(1)
        .WillRepeatedly(Return(expectedPaths));

    auto result = _man->getAllDownloads();
    QCOMPARE(3, result.count());
    foreach(auto path, result) {
        QVERIFY(expectedPaths.contains(path.path()));
    }

    QVERIFY(Mock::VerifyAndClearExpectations(dbusProxy));
    verifyMocks();
}

void
TestDownloadManager::testGetAllDownloadsConfined() {
    QString expectedAppId = "APPID";
    auto dbusProxy = new MockDBusProxy();
    auto reply = new MockPendingReply<QString>();

    EXPECT_CALL(*_dbusProxyFactory, createDBusProxy(_))
        .Times(1)
        .WillOnce(Return(dbusProxy));

    EXPECT_CALL(*dbusProxy, GetConnectionAppArmorSecurityContext(_))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, waitForFinished())
        .Times(1);

    EXPECT_CALL(*reply, isError())
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*reply, value())
        .Times(1)
        .WillOnce(Return(expectedAppId));

    // create several downloads with diff app ids to check that we
    // do getonly those with the correct app id
    QVariantMap metadata;
    QMap<QString, QString> headers;
    QScopedPointer<MockDownload> first(new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), metadata, headers));
    QScopedPointer<MockDownload> second(new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), metadata, headers));
    QScopedPointer<MockDownload> third(new MockDownload("", "", "", "",
         QUrl("http://reddit.com"), metadata, headers));

    QHash<QString, Transfer*> downs;
    QStringList paths;
    paths.append("/first/path/object");
    paths.append("/second/path/object");
    paths.append("/third/path/object");

    downs[paths[0]] = first.data();
    downs[paths[1]] = second.data();
    downs[paths[2]] = third.data();

    EXPECT_CALL(*first.data(), transferAppId())
        .Times(1)
        .WillRepeatedly(Return(expectedAppId));

    EXPECT_CALL(*second.data(), transferAppId())
        .Times(1)
        .WillRepeatedly(Return(QString("SECONDAPP")));

    EXPECT_CALL(*third.data(), transferAppId())
        .Times(1)
        .WillRepeatedly(Return(QString("LASTAPP")));

    EXPECT_CALL(*_q, transfers())
        .Times(1)
        .WillRepeatedly(Return(downs));

    auto result = _man->getAllDownloads();
    QCOMPARE(1, result.count());

    QVERIFY(Mock::VerifyAndClearExpectations(dbusProxy));
    verifyMocks();
}

void
TestDownloadManager::testAllDownloadsWithMetadataUnconfined() {
    QString expectedAppId = "unconfined";
    auto dbusProxy = new MockDBusProxy();
    auto reply = new MockPendingReply<QString>();

    EXPECT_CALL(*_dbusProxyFactory, createDBusProxy(_))
        .Times(1)
        .WillOnce(Return(dbusProxy));

    EXPECT_CALL(*dbusProxy, GetConnectionAppArmorSecurityContext(_))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, waitForFinished())
        .Times(1);

    EXPECT_CALL(*reply, isError())
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*reply, value())
        .Times(1)
        .WillOnce(Return(expectedAppId));

    // create downloads to be used to filter
    auto key = QString("filter");
    auto value = QString("coconut");
    auto validPath = QString("/valid/metadata/path");

    QVariantMap filteredMetadata;
    filteredMetadata[key] = value;
    QVariantMap metadata;
    QMap<QString, QString> headers;

    QScopedPointer<MockDownload> first(new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), metadata, headers));
    QScopedPointer<MockDownload> second(new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), metadata, headers));
    QScopedPointer<MockDownload> third(new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), metadata, headers));
    QHash<QString, Transfer*> downs;
    downs[validPath] = first.data();
    downs["/second/object/path"] = second.data();
    downs["/third/object/path"] = third.data();

    EXPECT_CALL(*first.data(), metadata())
        .Times(1)
        .WillRepeatedly(Return(filteredMetadata));

    EXPECT_CALL(*first.data(), transferAppId())
        .Times(0);

    EXPECT_CALL(*second.data(), metadata())
        .Times(1)
        .WillRepeatedly(Return(metadata));

    EXPECT_CALL(*second.data(), transferAppId())
        .Times(0);

    EXPECT_CALL(*third.data(), metadata())
        .Times(1)
        .WillRepeatedly(Return(metadata));

    EXPECT_CALL(*third.data(), transferAppId())
        .Times(0);

    EXPECT_CALL(*_q, transfers())
        .Times(1)
        .WillRepeatedly(Return(downs));

    auto result = _man->getAllDownloadsWithMetadata(key, value);
    QCOMPARE(1, result.count());
    QCOMPARE(result[0].path(), validPath);

    QVERIFY(Mock::VerifyAndClearExpectations(dbusProxy));
    verifyMocks();
}

void
TestDownloadManager::testAllDownloadsWithMetadataConfined() {
    QString expectedAppId = "APPID";
    auto dbusProxy = new MockDBusProxy();
    auto reply = new MockPendingReply<QString>();

    EXPECT_CALL(*_dbusProxyFactory, createDBusProxy(_))
        .Times(1)
        .WillOnce(Return(dbusProxy));

    EXPECT_CALL(*dbusProxy, GetConnectionAppArmorSecurityContext(_))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, waitForFinished())
        .Times(1);

    EXPECT_CALL(*reply, isError())
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*reply, value())
        .Times(1)
        .WillOnce(Return(expectedAppId));

    auto key = QString("filter");
    auto value = QString("coconut");
    auto validPath = QString("/valid/metadata/path");

    QVariantMap filteredMetadata;
    filteredMetadata[key] = value;
    QVariantMap metadata;
    QMap<QString, QString> headers;

    QScopedPointer<MockDownload> first(new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), metadata, headers));
    QScopedPointer<MockDownload> second(new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), metadata, headers));
    QScopedPointer<MockDownload> third(new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), metadata, headers));
    QHash<QString, Transfer*> downs;
    downs[validPath] = first.data();
    downs["/second/object/path"] = second.data();
    downs["/third/object/path"] = third.data();

    EXPECT_CALL(*first.data(), metadata())
        .Times(1)
        .WillRepeatedly(Return(filteredMetadata));

    EXPECT_CALL(*first.data(), transferAppId())
        .Times(1)
        .WillRepeatedly(Return(expectedAppId));

    EXPECT_CALL(*second.data(), metadata())
        .Times(1)
        .WillRepeatedly(Return(filteredMetadata));

    EXPECT_CALL(*second.data(), transferAppId())
        .Times(1)
        .WillRepeatedly(Return(QString("LEAPP")));

    EXPECT_CALL(*third.data(), metadata())
        .Times(1)
        .WillRepeatedly(Return(filteredMetadata));

    EXPECT_CALL(*third.data(), transferAppId())
        .Times(1)
        .WillRepeatedly(Return(QString("LEAPP")));

    EXPECT_CALL(*_q, transfers())
        .Times(1)
        .WillRepeatedly(Return(downs));

    auto result = _man->getAllDownloadsWithMetadata(key, value);
    QCOMPARE(1, result.count());
    QCOMPARE(result[0].path(), validPath);

    QVERIFY(Mock::VerifyAndClearExpectations(dbusProxy));
    verifyMocks();
}

QTEST_MAIN(TestDownloadManager)
