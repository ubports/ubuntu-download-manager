/*
 * Copyright 2014 Canonical Ltd.
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

#include <matchers.h>
#include <network_reply.h>

#include "test_mms_upload.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::AnyNumber;
using ::testing::Return;
using ::testing::AnyOf;

void
TestMmsUpload::init() {
    BaseTestCase::init();
    _id = "fake_id";
    _appId = "My APP";
    _path = "/dbus/path/upload";
    _isConfined = true;
    _rootPath = testDirectory();
    _filePath = dataDirectory() + "/otasigned.zip";

    _reqFactory = new MockRequestFactory();
    RequestFactory::setInstance(_reqFactory);
    _fileManager = new MockFileManager();
    FileManager::setInstance(_fileManager);
}

void
TestMmsUpload::cleanup() {
    BaseTestCase::cleanup();
    RequestFactory::deleteInstance();
    FileManager::deleteInstance();
}

void
TestMmsUpload::verifyMocks() {
    QVERIFY(Mock::VerifyAndClearExpectations(_reqFactory));
    QVERIFY(Mock::VerifyAndClearExpectations(_fileManager));
}

void
TestMmsUpload::testStartCorrectHeaders() {
    // use a fake file so that we can fail the open and test that error is
    // returned
    auto file = new MockFile("test");
    auto reply = new MockNetworkReply();

    // mocks expectations
    EXPECT_CALL(*_fileManager, createFile(_))
        .Times(1)
        .WillOnce(Return(file));

    EXPECT_CALL(*file, open(_))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(*file, close())
        .Times(1);

    EXPECT_CALL(*_reqFactory, post(
                RequestHasHeaderWithValue("Content-Type",
                    "application/vnd.wap.mms-message"), _))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, setReadBufferSize(_))
        .Times(1);

    auto upload = new MmsFileUpload(_id, _appId, _path, _isConfined, _rootPath,
        _url, _filePath, _metadata, _headers, _reqFactory);

    SignalBarrier spy(upload, SIGNAL(started(bool)));

    upload->startTransfer();  // change state

    QVERIFY(spy.ensureSignalEmitted());
    QTRY_COMPARE(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toBool());

    delete upload;

    verifyMocks();
}

QTEST_MAIN(TestMmsUpload)
