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

#include "test_base_download.h"

using ::testing::Mock;

void
TestBaseDownload::init() {
    BaseTestCase::init();
    _id = "download-id";
    _path = "path-to-be-used";
    _isConfined = false;
    _rootPath = "/root/path/to/use";
    _url = QUrl("http://test.com");
}

void
TestBaseDownload::testStartQueued() {
    QScopedPointer<MockDownload> down(
        new MockDownload(_id, _path, _isConfined, _rootPath, _url,
            _metadata, _headers));
    down->start();
    EXPECT_CALL(*down.data(), startDownload())
            .Times(0);

    QVERIFY(Mock::VerifyAndClearExpectations(down.data()));
}

void
TestBaseDownload::testStartNotQueued() {
    QScopedPointer<MockDownload> down(
        new MockDownload(_id, _path, _isConfined, _rootPath, _url,
            _metadata, _headers));
    down->setAddToQueue(false);

    EXPECT_CALL(*down.data(), startDownload())
            .Times(1);

    down->start();

    QVERIFY(Mock::VerifyAndClearExpectations(down.data()));
}

void
TestBaseDownload::testPauseQueued() {
    QScopedPointer<MockDownload> down(
        new MockDownload(_id, _path, _isConfined, _rootPath, _url,
            _metadata, _headers));

    EXPECT_CALL(*down.data(), pauseDownload())
            .Times(0);

    down->pause();

    QVERIFY(Mock::VerifyAndClearExpectations(down.data()));
}

void
TestBaseDownload::testPauseNotQueued() {
    QScopedPointer<MockDownload> down(
        new MockDownload(_id, _path, _isConfined, _rootPath, _url,
            _metadata, _headers));
    down->setAddToQueue(false);

    EXPECT_CALL(*down.data(), pauseDownload())
            .Times(1);

    down->pause();

    QVERIFY(Mock::VerifyAndClearExpectations(down.data()));
}

void
TestBaseDownload::testResumeQueued() {
    QScopedPointer<MockDownload> down(
        new MockDownload(_id, _path, _isConfined, _rootPath, _url,
            _metadata, _headers));
    down->resume();

    EXPECT_CALL(*down.data(), resumeDownload())
            .Times(0);

    QVERIFY(Mock::VerifyAndClearExpectations(down.data()));
}

void
TestBaseDownload::testResumeNotQueued() {
    QScopedPointer<MockDownload> down(
        new MockDownload(_id, _path, _isConfined, _rootPath, _url,
            _metadata, _headers));
    down->setAddToQueue(false);

    EXPECT_CALL(*down.data(), resumeDownload())
            .Times(1);

    down->resume();

    QVERIFY(Mock::VerifyAndClearExpectations(down.data()));
}

void
TestBaseDownload::testCancelQueued() {
    QScopedPointer<MockDownload> down(
        new MockDownload(_id, _path, _isConfined, _rootPath, _url,
            _metadata, _headers));
    down->cancel();

    EXPECT_CALL(*down.data(), cancelDownload())
            .Times(0);
    QVERIFY(Mock::VerifyAndClearExpectations(down.data()));
}

void
TestBaseDownload::testCancelNotQueued() {
    QScopedPointer<MockDownload> down(
        new MockDownload(_id, _path, _isConfined, _rootPath, _url,
            _metadata, _headers));
    down->setAddToQueue(false);

    EXPECT_CALL(*down.data(), cancelDownload())
            .Times(1);

    down->cancel();
    QVERIFY(Mock::VerifyAndClearExpectations(down.data()));
}

QTEST_MAIN(TestBaseDownload)
#include "moc_test_base_download.cpp"
