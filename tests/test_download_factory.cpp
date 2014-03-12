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

#include <ubuntu/downloads/file_download.h>
#include <ubuntu/downloads/mms_file_download.h>
#include <ubuntu/transfers/system/hash_algorithm.h>
#include <ubuntu/transfers/system/file_manager.h>
#include <ubuntu/transfers/system/uuid_utils.h>
#include "test_download_factory.h"

void
TestDownloadFactory::init() {
    BaseTestCase::init();
    _uuidFactory = QSharedPointer<UuidFactory>(new FakeUuidFactory());
    _apparmor = new MockAppArmor(_uuidFactory);
    _networkInfo =  new FakeSystemNetworkInfo();
    SystemNetworkInfo::setInstance(_networkInfo);
    _downFactory = new Factory(_apparmor);
}

void
TestDownloadFactory::cleanup() {
    BaseTestCase::cleanup();

    SystemNetworkInfo::deleteInstance();
    delete _downFactory;
    FileManager::deleteInstance();
    RequestFactory::deleteInstance();
}

void
TestDownloadFactory::testCreateDownload() {
    // create a download, assert that it was
    // created and that the id and the path are correctly set
    QScopedPointer<Download> download(_downFactory->createDownload("",
        QUrl(), QVariantMap(), QMap<QString, QString>()));

//    EXPECT_CALL(_apparmor, getSecurePath
    QList<MethodData> calledMethods = _apparmor->calledMethods();
    QCOMPARE(1, calledMethods.count());
    StringWrapper* id = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[0]);
    StringWrapper* path = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[1]);
    QCOMPARE(download->downloadId(), id->value());
    QCOMPARE(download->path(), path->value());
}

void
TestDownloadFactory::testCreateDownloadWithHash() {
    QString hash = "my-hash";
    QString algo = "Md5";

    // same as above but assert hash and hash algo
    QScopedPointer<Download> download(_downFactory->createDownload("", QUrl(),
        hash, algo, QVariantMap(), QMap<QString, QString>()));

    QList<MethodData> calledMethods = _apparmor->calledMethods();
    QCOMPARE(1, calledMethods.count());
    StringWrapper* id = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[0]);
    StringWrapper* path = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[1]);
    QCOMPARE(download->downloadId(), id->value());
    QCOMPARE(download->path(), path->value());

    // no need to worry about the pointer because it will be
    // deleted by the QScopedPointer
    FileDownload* single = reinterpret_cast<FileDownload*>(download.data());
    QCOMPARE(hash, single->hash());
    QCOMPARE(HashAlgorithm::getHashAlgo(algo), single->hashAlgorithm());
}

void
TestDownloadFactory::testCreateMmsDownload() {
    QString hash = "my-hash";
    QString algo = "Md5";
    QString hostname = "http://hostname.com";
    int port = 88;
    QString username = "username";
    QString password = "password";


    // same as above but assert hash and hash algo
    QScopedPointer<Download> download(_downFactory->createMmsDownload(
        "", QUrl("http://example.com"), hostname, port, username, password));
    auto mms = qobject_cast<MmsFileDownload*>(download.data());
    QVERIFY(mms != nullptr);
}

void
TestDownloadFactory::testCreateGroupDownload() {
    // create a download, assert that it was
    // created and that the id and the path are correctly set
    QScopedPointer<Download> download(_downFactory->createDownload("",
        QList<GroupDownloadStruct>(), "Md5",
        true, QVariantMap(), QMap<QString, QString>()));

    QList<MethodData> calledMethods = _apparmor->calledMethods();
    QCOMPARE(1, calledMethods.count());
    StringWrapper* id = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[0]);
    StringWrapper* path = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[1]);
    QCOMPARE(download->downloadId(), id->value());
    QCOMPARE(download->path(), path->value());
}

void
TestDownloadFactory::testCreateDownloadWithValidUuid() {
    // create a download, assert that it was
    // created and that the id and the path are correctly set
    QString id = UuidUtils::getDBusString(QUuid::createUuid());

    QVariantMap metadata;
    metadata["objectpath"] = id;

    QScopedPointer<Download> download(_downFactory->createDownload("", QUrl(),
        metadata, QMap<QString, QString>()));

    QList<MethodData> calledMethods = _apparmor->calledMethods();
    QCOMPARE(1, calledMethods.count());
    StringWrapper* path = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[1]);
    QCOMPARE(download->downloadId(), id);
    QCOMPARE(download->path(), path->value());
}

void
TestDownloadFactory::testCreateDownloadWithNullUuid() {
    // create a download, assert that it was
    // created and that the id and the path are correctly set
    QVariantMap metadata;
    metadata["objectpath"] = "bad-id";

    QScopedPointer<Download> download(_downFactory->createDownload("", QUrl(),
        QVariantMap(), QMap<QString, QString>()));

    QList<MethodData> calledMethods = _apparmor->calledMethods();
    QCOMPARE(1, calledMethods.count());
    StringWrapper* id = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[0]);
    StringWrapper* path = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[1]);
    QCOMPARE(download->downloadId(), id->value());
    QCOMPARE(download->path(), path->value());
}

void
TestDownloadFactory::testCreateDownloadWithHashAndUuid() {
    QString id = UuidUtils::getDBusString(QUuid::createUuid());

    QVariantMap metadata;
    metadata["objectpath"] = id;

    QString hash = "my-hash";
    QString algo = "Md5";

    // same as above but assert hash and hash algo
    QScopedPointer<Download> download(_downFactory->createDownload("", QUrl(),
        hash, algo, metadata, QMap<QString, QString>()));

    QList<MethodData> calledMethods = _apparmor->calledMethods();
    QCOMPARE(1, calledMethods.count());
    StringWrapper* path = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[1]);
    QCOMPARE(download->downloadId(), id);
    QCOMPARE(download->path(), path->value());

    // not to worry, QSCopedPointer will take care of the pointer
    FileDownload* single = reinterpret_cast<FileDownload*>(download.data());
    QCOMPARE(hash, single->hash());
    QCOMPARE(HashAlgorithm::getHashAlgo(algo), single->hashAlgorithm());
}

void
TestDownloadFactory::testCreateDownloadWithHashAndNullUuid() {
    QVariantMap metadata;
    metadata["objectpath"] = "bad-id";

    QString hash = "my-hash";
    QString algo = "Md5";

    // same as above but assert hash and hash algo
    QScopedPointer<Download> download(_downFactory->createDownload("", QUrl(),
        hash, algo, metadata, QMap<QString, QString>()));

    QList<MethodData> calledMethods = _apparmor->calledMethods();
    QCOMPARE(1, calledMethods.count());
    StringWrapper* id = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[0]);
    StringWrapper* path = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[1]);
    QCOMPARE(download->downloadId(), id->value());
    QCOMPARE(download->path(), path->value());

    // not to worry the QScopedPointer will take care of the pointer
    FileDownload* single = reinterpret_cast<FileDownload*>(download.data());
    QCOMPARE(hash, single->hash());
    QCOMPARE(HashAlgorithm::getHashAlgo(algo), single->hashAlgorithm());
}

void
TestDownloadFactory::testCreateGroupDownloadWithValidUuid() {
    // create a download, assert that it was
    // created and that the id and the path are correctly set
    QString id = UuidUtils::getDBusString(QUuid::createUuid());

    QVariantMap metadata;
    metadata["objectpath"] = id;

    QScopedPointer<Download> download(_downFactory->createDownload("",
        QList<GroupDownloadStruct>(), "Md5",
        true, metadata, QMap<QString, QString>()));

    QList<MethodData> calledMethods = _apparmor->calledMethods();
    QCOMPARE(1, calledMethods.count());
    StringWrapper* path = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[1]);
    QCOMPARE(download->downloadId(), id);
    QCOMPARE(download->path(), path->value());
}

void
TestDownloadFactory::testCreateGroupDownloadWithNullUuid() {
    // create a download, assert that it was
    // created and that the id and the path are correctly set
    QVariantMap metadata;
    metadata["objectpath"] = "bad-id";

    QScopedPointer<Download> download(_downFactory->createDownload("",
        QList<GroupDownloadStruct>(), "Md5",
        true, metadata, QMap<QString, QString>()));

    QList<MethodData> calledMethods = _apparmor->calledMethods();
    QCOMPARE(1, calledMethods.count());
    StringWrapper* id = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[0]);
    StringWrapper* path = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[1]);
    QCOMPARE(download->downloadId(), id->value());
    QCOMPARE(download->path(), path->value());
}

void
TestDownloadFactory::testCreateDownloadForGroup() {
    QScopedPointer<Download> download(_downFactory->createDownloadForGroup(true, "", QUrl(),
        QVariantMap(), QMap<QString, QString>()));

    QList<MethodData> calledMethods = _apparmor->calledMethods();
    QCOMPARE(1, calledMethods.count());
    StringWrapper* id = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[0]);
    StringWrapper* path = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[1]);

    QCOMPARE(download->downloadId(), id->value());
    QCOMPARE(download->path(), path->value());
}

void
TestDownloadFactory::testCreateDownloadForGroupWithHash() {
    QScopedPointer<Download> download(_downFactory->createDownloadForGroup(true, "", QUrl(),
        "", "Md5", QVariantMap(), QMap<QString, QString>()));

    QList<MethodData> calledMethods = _apparmor->calledMethods();
    QCOMPARE(1, calledMethods.count());
    StringWrapper* id = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[0]);
    StringWrapper* path = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().outParams()[1]);

    QCOMPARE(download->downloadId(), id->value());
    QCOMPARE(download->path(), path->value());
}

QTEST_MAIN(TestDownloadFactory)
