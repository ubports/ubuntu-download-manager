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
#include <ubuntu/downloads/group_download.h>
#include <ubuntu/transfers/system/hash_algorithm.h>
#include <ubuntu/transfers/system/file_manager.h>
#include <ubuntu/transfers/system/uuid_utils.h>
#include "test_download_factory.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Return;

void
TestDownloadFactory::init() {
    BaseTestCase::init();
    _apparmor = new MockAppArmor();
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
    auto id = QString("id");
    auto busPath = QString("/com/dbus/path");
    auto details = new SecurityDetails(id);
    details->dbusPath = busPath;
    details->localPath = "/local/path";
    details->isConfined = true;

    EXPECT_CALL(*_apparmor, getSecurityDetails(_))
        .Times(1)
        .WillRepeatedly(Return(details));

    // create a download, assert that it was
    // created and that the id and the path are correctly set
    QScopedPointer<Download> download(_downFactory->createDownload("",
        QUrl(), QVariantMap(), QMap<QString, QString>()));

    QCOMPARE(download->downloadId(), id);
    QCOMPARE(download->path(), busPath);

    QVERIFY(Mock::VerifyAndClearExpectations(_apparmor));
}

void
TestDownloadFactory::testCreateDownloadWithHash() {
    auto hash = QString("my-hash");
    auto algo = QString("Md5");
    auto id = QString("id");
    auto busPath = QString("/com/dbus/path");
    auto details = new SecurityDetails(id);
    details->dbusPath = busPath;
    details->localPath = "/local/path";
    details->isConfined = true;

    EXPECT_CALL(*_apparmor, getSecurityDetails(_))
        .Times(1)
        .WillRepeatedly(Return(details));

    // same as above but assert hash and hash algo
    QScopedPointer<Download> download(_downFactory->createDownload("", QUrl(),
        hash, algo, QVariantMap(), QMap<QString, QString>()));

    QCOMPARE(download->downloadId(), id);
    QCOMPARE(download->path(), busPath);

    // no need to worry about the pointer because it will be
    // deleted by the QScopedPointer
    FileDownload* single = reinterpret_cast<FileDownload*>(download.data());
    QCOMPARE(hash, single->hash());
    QCOMPARE(HashAlgorithm::getHashAlgo(algo), single->hashAlgorithm());

    QVERIFY(Mock::VerifyAndClearExpectations(_apparmor));
}

void
TestDownloadFactory::testCreateMmsDownload() {
    QString hash = "my-hash";
    QString algo = "Md5";
    QString hostname = "http://hostname.com";
    int port = 88;
    QString username = "username";
    QString password = "password";
    auto id = QString("id");
    auto busPath = QString("/com/dbus/path");
    auto details = new SecurityDetails(id);
    details->dbusPath = busPath;
    details->localPath = "/local/path";
    details->isConfined = true;

    EXPECT_CALL(*_apparmor, getSecurityDetails(_))
        .Times(1)
        .WillRepeatedly(Return(details));

    // same as above but assert hash and hash algo
    QScopedPointer<Download> download(_downFactory->createMmsDownload(
        "", QUrl("http://example.com"), hostname, port, username, password));
    auto mms = qobject_cast<MmsFileDownload*>(download.data());
    QVERIFY(mms != nullptr);
    QVERIFY(Mock::VerifyAndClearExpectations(_apparmor));
}

void
TestDownloadFactory::testCreateGroupDownload() {
    auto id = QString("id");
    auto busPath = QString("/com/dbus/path");
    auto details = new SecurityDetails(id);
    details->dbusPath = busPath;
    details->localPath = "/local/path";
    details->isConfined = true;

    EXPECT_CALL(*_apparmor, getSecurityDetails(_))
        .Times(1)
        .WillRepeatedly(Return(details));

    // create a download, assert that it was
    // created and that the id and the path are correctly set
    QScopedPointer<Download> download(_downFactory->createDownload("",
        QList<GroupDownloadStruct>(), "Md5",
        true, QVariantMap(), QMap<QString, QString>()));

    QCOMPARE(download->downloadId(), id);
    QCOMPARE(download->path(), busPath);

    auto group = qobject_cast<GroupDownload*>(download.data());
    QVERIFY(group != nullptr);
    QVERIFY(Mock::VerifyAndClearExpectations(_apparmor));
}

void
TestDownloadFactory::testCreateDownloadWithValidUuid() {
    // create a download, assert that it was
    // created and that the id and the path are correctly set
    QString id = UuidUtils::getDBusString(QUuid::createUuid());
    QVariantMap metadata;
    metadata["objectpath"] = id;
    auto busPath = QString("/com/dbus/path");
    auto details = new SecurityDetails(id);
    details->dbusPath = busPath;
    details->localPath = "/local/path";
    details->isConfined = true;

    EXPECT_CALL(*_apparmor, getSecurityDetails(_, id))
        .Times(1)
        .WillRepeatedly(Return(details));

    QScopedPointer<Download> download(_downFactory->createDownload("", QUrl(),
        metadata, QMap<QString, QString>()));

    QCOMPARE(download->downloadId(), id);
    QCOMPARE(download->path(), busPath);

    QVERIFY(Mock::VerifyAndClearExpectations(_apparmor));
}

void
TestDownloadFactory::testCreateDownloadWithHashAndUuid() {
    QString id = UuidUtils::getDBusString(QUuid::createUuid());
    QVariantMap metadata;
    metadata["objectpath"] = id;
    QString hash = "my-hash";
    QString algo = "Md5";

    auto busPath = QString("/com/dbus/path");
    auto details = new SecurityDetails(id);
    details->dbusPath = busPath;
    details->localPath = "/local/path";
    details->isConfined = true;

    EXPECT_CALL(*_apparmor, getSecurityDetails(_, id))
        .Times(1)
        .WillRepeatedly(Return(details));

    // same as above but assert hash and hash algo
    QScopedPointer<Download> download(_downFactory->createDownload("", QUrl(),
        hash, algo, metadata, QMap<QString, QString>()));

    QCOMPARE(download->downloadId(), id);
    QCOMPARE(download->path(), busPath);

    // not to worry, QSCopedPointer will take care of the pointer
    auto single = reinterpret_cast<FileDownload*>(download.data());
    QCOMPARE(hash, single->hash());
    QCOMPARE(HashAlgorithm::getHashAlgo(algo), single->hashAlgorithm());

    QVERIFY(Mock::VerifyAndClearExpectations(_apparmor));
}

void
TestDownloadFactory::testCreateGroupDownloadWithValidUuid() {
    // create a download, assert that it was
    // created and that the id and the path are correctly set
    QString id = UuidUtils::getDBusString(QUuid::createUuid());

    QVariantMap metadata;
    metadata["objectpath"] = id;
    auto busPath = QString("/com/dbus/path");
    auto details = new SecurityDetails(id);
    details->dbusPath = busPath;
    details->localPath = "/local/path";
    details->isConfined = true;

    EXPECT_CALL(*_apparmor, getSecurityDetails(_, id))
        .Times(1)
        .WillRepeatedly(Return(details));

    QScopedPointer<Download> download(_downFactory->createDownload("",
        QList<GroupDownloadStruct>(), "Md5",
        true, metadata, QMap<QString, QString>()));

    QCOMPARE(download->downloadId(), id);
    QCOMPARE(download->path(), busPath);
    QVERIFY(Mock::VerifyAndClearExpectations(_apparmor));
}

void
TestDownloadFactory::testCreateDownloadForGroup() {
    auto id = QString("my id");
    auto busPath = QString("/com/dbus/path");
    QPair<QString, QString> pair(id, busPath);

    EXPECT_CALL(*_apparmor, getDBusPath())
        .Times(1)
        .WillRepeatedly(Return(pair));
    QScopedPointer<Download> download(_downFactory->createDownloadForGroup(
        true, "", QUrl(), QVariantMap(), QMap<QString, QString>()));

    QCOMPARE(download->downloadId(), id);
    QCOMPARE(download->path(), busPath);
    QVERIFY(Mock::VerifyAndClearExpectations(_apparmor));
}

void
TestDownloadFactory::testCreateDownloadForGroupWithHash() {
    QString hash = "my-hash";
    QString algo = "Md5";
    auto id = QString("my id");
    auto busPath = QString("/com/dbus/path");
    QPair<QString, QString> pair(id, busPath);

    EXPECT_CALL(*_apparmor, getDBusPath())
        .Times(1)
        .WillRepeatedly(Return(pair));

    QScopedPointer<Download> download(_downFactory->createDownloadForGroup(
        true, "", QUrl(), hash, algo, QVariantMap(),
        QMap<QString, QString>()));

    QCOMPARE(download->downloadId(), id);
    QCOMPARE(download->path(), busPath);

    auto single = reinterpret_cast<FileDownload*>(download.data());
    QCOMPARE(hash, single->hash());
    QCOMPARE(HashAlgorithm::getHashAlgo(algo), single->hashAlgorithm());

    QVERIFY(Mock::VerifyAndClearExpectations(_apparmor));
}

QTEST_MAIN(TestDownloadFactory)
