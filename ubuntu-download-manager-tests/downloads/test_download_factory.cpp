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
#include <ubuntu/system/hash_algorithm.h>
#include <ubuntu/system/file_manager.h>
#include <ubuntu/system/uuid_utils.h>
#include "test_download_factory.h"

TestDownloadFactory::TestDownloadFactory(QObject *parent)
    : BaseTestCase("TestDownloadFactory", parent) {
}

void
TestDownloadFactory::init() {
    BaseTestCase::init();
    _uuidFactory = QSharedPointer<UuidFactory>(new FakeUuidFactory());
    _apparmor = new FakeAppArmor(_uuidFactory);
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
    _apparmor->record();

    // create a download, assert that it was
    // created and that the id and the path are correctly set
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
TestDownloadFactory::testCreateDownloadWithHash() {
    _apparmor->record();

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
    _apparmor->record();

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
    _apparmor->record();

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
    _apparmor->record();

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
    _apparmor->record();

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
    _apparmor->record();

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
    _apparmor->record();

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
    _apparmor->record();

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
    _apparmor->record();

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
    _apparmor->record();
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
    _apparmor->record();
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
