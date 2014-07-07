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

#include <ubuntu/uploads/mms_file_upload.h>
#include "test_upload_factory.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::Return;

void
TestUploadFactory::init() {
    BaseTestCase::init();
    _apparmor = new MockAppArmor();
    _upFactory = new Factory(_apparmor);
}

void
TestUploadFactory::cleanup() {
    BaseTestCase::cleanup();

    SystemNetworkInfo::deleteInstance();
    delete _upFactory;
    FileManager::deleteInstance();
    RequestFactory::deleteInstance();
}

void
TestUploadFactory::testCreateUpload() {
    auto id = QString("id");
    auto busPath = QString("/com/dbus/path/%1");
    auto details = new SecurityDetails(id);
    details->dbusPath = busPath;
    details->localPath = "/local/path";
    details->isConfined = true;

    EXPECT_CALL(*_apparmor, getSecurityDetails(_))
        .Times(1)
        .WillRepeatedly(Return(details));

    // create a download, assert that it was
    // created and that the id and the path are correctly set
    QScopedPointer<FileUpload> upload(_upFactory->createUpload("",
        QUrl(), "/tmp", QVariantMap(), QMap<QString, QString>()));

    QCOMPARE(upload->transferId(), id);
    QCOMPARE(upload->path(), busPath.arg("upload"));

    QVERIFY(Mock::VerifyAndClearExpectations(_apparmor));
}

void
TestUploadFactory::testCreateMmsUpload() {
    QString hash = "my-hash";
    QString algo = "Md5";
    QString hostname = "http://hostname.com";
    int port = 88;
    auto id = QString("id");
    auto busPath = QString("/com/dbus/path/%1");
    auto details = new SecurityDetails(id);
    details->dbusPath = busPath;
    details->localPath = "/local/path";
    details->isConfined = true;

    EXPECT_CALL(*_apparmor, getSecurityDetails(_))
        .Times(1)
        .WillRepeatedly(Return(details));

    // same as above but assert hash and hash algo
    QScopedPointer<FileUpload> upload(_upFactory->createMmsUpload(
        "", QUrl("http://example.com"), "/tmp", hostname, port));
    auto mms = qobject_cast<MmsFileUpload*>(upload.data());
    QVERIFY(mms != nullptr);
    QVERIFY(Mock::VerifyAndClearExpectations(_apparmor));
}

QTEST_MAIN(TestUploadFactory)
