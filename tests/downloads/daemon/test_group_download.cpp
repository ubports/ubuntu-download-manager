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

#include <QDebug>
#include <QScopedPointer>
#include <ubuntu/transfers/metadata.h>
#include <ubuntu/downloads/group_download.h>
#include <ubuntu/transfers/system/uuid_utils.h>
#include "apparmor.h"
#include "dbus_connection.h"
#include "download.h"
#include "matchers.h"
#include "test_group_download.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::AnyNumber;
using ::testing::Return;
using namespace Ubuntu::Transfers;

void
TestGroupDownload::init() {
    BaseTestCase::init();
    _id = UuidUtils::getDBusString(QUuid::createUuid());
    _appId = "Testing App";
    _path = "/group/dbus/path";
    _isConfined = false;
    _rootPath = "/random/dbus/path";
    _algo = "Md5";
    _isGSMDownloadAllowed = true;
    _factory = new MockDownloadFactory(new MockAppArmor(new MockDBusConnection()));
    _fileManager = new MockFileManager();
    FileManager::setInstance(_fileManager);
}

void
TestGroupDownload::verifyMocks() {
    QVERIFY(Mock::VerifyAndClearExpectations(_factory));
    QVERIFY(Mock::VerifyAndClearExpectations(_fileManager));
}

void
TestGroupDownload::cleanup() {
    BaseTestCase::cleanup();

    NetworkSession::deleteInstance();
    FileManager::deleteInstance();
    delete _factory;
}

void
TestGroupDownload::testCancelNoDownloads() {
    QList<GroupDownloadStruct> downloads;

    GroupDownload* group = new GroupDownload(_id, _appId, _path, _isConfined,
        _rootPath, downloads, _algo, _isGSMDownloadAllowed, _metadata,
        _headers, _factory, _fileManager);
    group->cancelTransfer();
    verifyMocks();
}

void
TestGroupDownload::testCancelAllDownloads() {
    auto first = new MockDownload("", "", "", "", QUrl(),
        _metadata, _headers);
    auto second = new MockDownload("", "", "", "", QUrl(),
        _metadata, _headers);
    QList<MockDownload*> downs;
    downs.append(first);
    downs.append(second);

    QList<GroupDownloadStruct> downloads;
    downloads.append(GroupDownloadStruct("http://one.ubunt.com",
        "local_file", ""));
    downloads.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));

    // set the expectations

    EXPECT_CALL(*_factory, createDownloadForGroup(_, _, _, _, _))
        .Times(AnyNumber())
        .WillOnce(Return(first))
        .WillOnce(Return(second));

    auto index = 0;
    foreach(MockDownload* down, downs) {
        EXPECT_CALL(*down, isValid())
            .Times(1)
            .WillRepeatedly(Return(true));
        EXPECT_CALL(*down, filePath())
            .Times(1)
            .WillRepeatedly(Return(QString("local_file %1").arg(index)));
        EXPECT_CALL(*down, state())
            .Times(1)
            .WillOnce(Return(Download::START));
        EXPECT_CALL(*down, cancelTransfer())
            .Times(1);
        index++;
    }

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloads, _algo, _isGSMDownloadAllowed,
        _metadata, _headers, _factory, _fileManager));
    group->cancelTransfer();

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testCancelDownloadWithFinished() {
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);
    auto second = new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), _metadata, _headers);
    auto third = new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), _metadata, _headers);
    QList<MockDownload*> downs;
    downs.append(first);
    downs.append(second);
    downs.append(third);

    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        "my_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    // set the expectations

    EXPECT_CALL(*_factory, createDownloadForGroup(_, _, _, _, _))
        .Times(AnyNumber())
        .WillOnce(Return(first))
        .WillOnce(Return(second))
        .WillOnce(Return(third));

    auto index = 0;
    foreach(MockDownload* down, downs) {
        EXPECT_CALL(*down, isValid())
            .Times(1)
            .WillRepeatedly(Return(true));
        EXPECT_CALL(*down, filePath())
            .Times(1)
            .WillRepeatedly(Return(QString("local_file %1").arg(index)));
        EXPECT_CALL(*down, state())
            .Times(1)
            .WillOnce(Return(Download::START));
        EXPECT_CALL(*down, cancelTransfer())
            .Times(1);
        index++;
    }

    auto downloadedPath = QString("downloaded");
    // we expect the downloadedPath to be removed from the fs

    EXPECT_CALL(*_fileManager, remove(downloadedPath))
        .Times(1)
        .WillOnce(Return(true));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    first->finished(downloadedPath);
    group->cancelTransfer();

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testCancelDownloadWithCancel() {
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);
    auto second = new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), _metadata, _headers);
    auto third = new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), _metadata, _headers);
    QList<MockDownload*> downs;
    downs.append(first);
    downs.append(second);
    downs.append(third);

    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        "my_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    // set the expectations

    EXPECT_CALL(*_factory, createDownloadForGroup(_, _, _, _, _))
        .Times(AnyNumber())
        .WillOnce(Return(first))
        .WillOnce(Return(second))
        .WillOnce(Return(third));

    for(auto index = 1; index < downs.count(); index++) {
        EXPECT_CALL(*downs[index], isValid())
            .Times(1)
            .WillRepeatedly(Return(true));
        EXPECT_CALL(*downs[index], filePath())
            .Times(1)
            .WillRepeatedly(Return(QString("local_file %1").arg(index)));
        EXPECT_CALL(*downs[index], state())
            .Times(1)
            .WillOnce(Return(Download::START));
        EXPECT_CALL(*downs[index], cancelTransfer())
            .Times(1);
    }

    // set the expectation for the first download which is a little diff
    EXPECT_CALL(*downs[0], isValid())
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*downs[0], filePath())
        .Times(1)
        .WillRepeatedly(Return(QString("local_file")));
    EXPECT_CALL(*downs[0], state())
        .Times(1)
        .WillOnce(Return(Download::CANCEL));
    EXPECT_CALL(*downs[0], cancelTransfer())
        .Times(0);

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    // cancel a download and the expectations will make sue that we are
    // only calling cancelTransfer once
    group->cancelTransfer();

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testPauseNoDownloads() {
    QList<GroupDownloadStruct> downloads;

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloads, _algo, _isGSMDownloadAllowed,
        _metadata, _headers, _factory, _fileManager));
    group->pauseTransfer();
    verifyMocks();
}

void
TestGroupDownload::testPauseAllDownloads() {
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);
    auto second = new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), _metadata, _headers);
    auto third = new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), _metadata, _headers);
    QList<MockDownload*> downs;
    downs.append(first);
    downs.append(second);
    downs.append(third);

    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        "my_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    // set the expectations

    EXPECT_CALL(*_factory, createDownloadForGroup(_, _, _, _, _))
        .Times(AnyNumber())
        .WillOnce(Return(first))
        .WillOnce(Return(second))
        .WillOnce(Return(third));

    auto index = 0;
    foreach(MockDownload* down, downs) {
        EXPECT_CALL(*down, isValid())
            .Times(1)
            .WillRepeatedly(Return(true));
        EXPECT_CALL(*down, filePath())
            .Times(1)
            .WillRepeatedly(Return(QString("local_file %1").arg(index)));
        EXPECT_CALL(*down, startTransfer())
            .Times(1);
        EXPECT_CALL(*down, state())
            .Times(2)
            .WillOnce(Return(Download::IDLE))
            .WillOnce(Return(Download::START));
        EXPECT_CALL(*down, pauseTransfer())
            .Times(1);
        index++;
    }

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));
    group->startTransfer();

    // trust mocks to ensure that pauseTransfer was called in all downloads
    group->pauseTransfer();

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testPauseDownloadWithFinished() {
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);
    auto path = QString("downloadedPath");
    auto second = new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), _metadata, _headers);
    auto third = new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), _metadata, _headers);
    QList<MockDownload*> downs;
    downs.append(second);
    downs.append(third);

    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        "my_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    // set the expectations

    EXPECT_CALL(*_factory, createDownloadForGroup(_, _, _, _, _))
        .Times(AnyNumber())
        .WillOnce(Return(first))
        .WillOnce(Return(second))
        .WillOnce(Return(third));

    // set diff expectation for that download that finishes (first)
    EXPECT_CALL(*first, isValid())
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*first, filePath())
        .Times(1)
        .WillRepeatedly(Return(QString("first_local_file")));
    EXPECT_CALL(*first, startTransfer())
        .Times(1);
    EXPECT_CALL(*first, state())
        .Times(2)
        .WillOnce(Return(Download::IDLE))
        .WillOnce(Return(Download::FINISH));

    // set the expectations for all other downloads.
    auto index = 0;
    foreach(MockDownload* down, downs) {
        EXPECT_CALL(*down, isValid())
            .Times(1)
            .WillRepeatedly(Return(true));
        EXPECT_CALL(*down, filePath())
            .Times(1)
            .WillRepeatedly(Return(QString("local_file %1").arg(index)));
        EXPECT_CALL(*down, startTransfer())
            .Times(1);
        EXPECT_CALL(*down, state())
            .Times(2)
            .WillOnce(Return(Download::IDLE))
            .WillOnce(Return(Download::START));
        EXPECT_CALL(*down, pauseTransfer())
            .Times(1);
        index++;
    }

    EXPECT_CALL(*_fileManager, remove(path))
        .Times(0);

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    group->startTransfer();
    first->finished(path);
    group->pauseTransfer();

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testPauseDownloadWithCancel() {
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);
    auto path = QString("downloadedPath");
    auto second = new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), _metadata, _headers);
    auto third = new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), _metadata, _headers);
    QList<MockDownload*> downs;
    downs.append(first);
    downs.append(second);
    downs.append(third);

    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        "my_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    // set the expectations

    EXPECT_CALL(*_factory, createDownloadForGroup(_, _, _, _, _))
        .Times(3)
        .WillOnce(Return(first))
        .WillOnce(Return(second))
        .WillOnce(Return(third));

    // set the expectations for all other downloads.
    for(auto index = 1; index < downs.count(); index ++) {
        EXPECT_CALL(*downs[index], isValid())
            .Times(1)
            .WillRepeatedly(Return(true));
        auto path = QString("local_file %1").arg(index);
        EXPECT_CALL(*downs[index], filePath())
            .Times(1)
            .WillRepeatedly(Return(path));
        EXPECT_CALL(*downs[index], startTransfer())
            .Times(1);
        EXPECT_CALL(*downs[index], state())
            .Times(2)
            .WillOnce(Return(Download::IDLE))
            .WillOnce(Return(Download::START));
        EXPECT_CALL(*downs[index], pauseTransfer())
            .Times(0);
        EXPECT_CALL(*downs[index], cancelTransfer())
            .Times(1);
    }

    // we are going to tell first to be canceled manually
    EXPECT_CALL(*downs[0], isValid())
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*downs[0], filePath())
        .Times(1)
        .WillRepeatedly(Return(QString("local_file")));
    EXPECT_CALL(*downs[0], startTransfer())
        .Times(1);
    EXPECT_CALL(*downs[0], state())
        .Times(2)
        .WillOnce(Return(Download::IDLE))
        .WillOnce(Return(Download::CANCEL));
    EXPECT_CALL(*downs[0], pauseTransfer())
        .Times(0);
    EXPECT_CALL(*_fileManager, remove(path))
        .Times(0);

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    group->startTransfer();
    first->canceled(true);

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testResumeNoDownloads() {
    QList<GroupDownloadStruct> downloads;

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloads, _algo, _isGSMDownloadAllowed,
        _metadata, _headers, _factory, _fileManager));
    group->cancelTransfer();
    verifyMocks();
}

void
TestGroupDownload::testResumeAllDownloads() {
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);
    auto path = QString("downloadedPath");
    auto second = new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), _metadata, _headers);
    auto third = new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), _metadata, _headers);
    QList<MockDownload*> downs;
    downs.append(first);
    downs.append(second);
    downs.append(third);

    // set the expectations

    EXPECT_CALL(*_factory, createDownloadForGroup(_, _, _, _, _))
        .Times(3)
        .WillOnce(Return(first))
        .WillOnce(Return(second))
        .WillOnce(Return(third));

    // set the expectations for all other downloads.
    auto index = 0;
    foreach(auto down, downs) {
        EXPECT_CALL(*down, isValid())
            .Times(1)
            .WillRepeatedly(Return(true));
        auto path = QString("local_file %1").arg(index);
        EXPECT_CALL(*down, filePath())
            .Times(1)
            .WillRepeatedly(Return(path));
        EXPECT_CALL(*down, state())
            .Times(1)
            .WillOnce(Return(Download::PAUSE));
        EXPECT_CALL(*down, resumeTransfer())
            .Times(1);
        index++;
    }

    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    group->resumeTransfer();

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testResumeWithFinished() {
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);
    auto path = QString("downloadedPath");
    auto second = new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), _metadata, _headers);
    auto third = new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), _metadata, _headers);
    QList<MockDownload*> downs;
    downs.append(first);
    downs.append(second);
    downs.append(third);

    // set the expectations

    EXPECT_CALL(*_factory, createDownloadForGroup(_, _, _, _, _))
        .Times(3)
        .WillOnce(Return(first))
        .WillOnce(Return(second))
        .WillOnce(Return(third));

    for(auto index = 1; index < downs.count(); index++) {
        EXPECT_CALL(*downs[index], isValid())
            .Times(1)
            .WillRepeatedly(Return(true));
        auto path = QString("local_file %1").arg(index);
        EXPECT_CALL(*downs[index], filePath())
            .Times(1)
            .WillRepeatedly(Return(path));
        EXPECT_CALL(*downs[index], state())
            .Times(1)
            .WillOnce(Return(Download::PAUSE));
        EXPECT_CALL(*downs[index], resumeTransfer())
            .Times(1);
    }

    EXPECT_CALL(*downs[0], isValid())
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*downs[0], filePath())
        .Times(1)
        .WillRepeatedly(Return(QString("path")));
    EXPECT_CALL(*downs[0], state())
        .Times(1)
        .WillOnce(Return(Download::FINISH));
    EXPECT_CALL(*downs[0], resumeTransfer())
        .Times(0);

    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    group->resumeTransfer();

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testResumeWidhCancel() {
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);
    auto path = QString("downloadedPath");
    auto second = new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), _metadata, _headers);
    auto third = new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), _metadata, _headers);
    QList<MockDownload*> downs;
    downs.append(first);
    downs.append(second);
    downs.append(third);

    // set the expectations

    EXPECT_CALL(*_factory, createDownloadForGroup(_, _, _, _, _))
        .Times(3)
        .WillOnce(Return(first))
        .WillOnce(Return(second))
        .WillOnce(Return(third));

    for(auto index = 1; index < downs.count(); index++) {
        EXPECT_CALL(*downs[index], isValid())
            .Times(1)
            .WillRepeatedly(Return(true));
        auto path = QString("local_file %1").arg(index);
        EXPECT_CALL(*downs[index], filePath())
            .Times(1)
            .WillRepeatedly(Return(path));
        EXPECT_CALL(*downs[index], state())
            .Times(1)
            .WillOnce(Return(Download::PAUSE));
        EXPECT_CALL(*downs[index], resumeTransfer())
            .Times(1);
    }

    EXPECT_CALL(*downs[0], isValid())
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*downs[0], filePath())
        .Times(1)
        .WillRepeatedly(Return(QString("path")));
    EXPECT_CALL(*downs[0], state())
        .Times(1)
        .WillOnce(Return(Download::CANCEL));
    EXPECT_CALL(*downs[0], resumeTransfer())
        .Times(0);

    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    group->resumeTransfer();

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testResumeNoStarted() {
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);
    auto path = QString("downloadedPath");
    auto second = new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), _metadata, _headers);
    auto third = new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), _metadata, _headers);
    QList<MockDownload*> downs;
    downs.append(first);
    downs.append(second);
    downs.append(third);

    // set the expectations

    EXPECT_CALL(*_factory, createDownloadForGroup(_, _, _, _, _))
        .Times(3)
        .WillOnce(Return(first))
        .WillOnce(Return(second))
        .WillOnce(Return(third));

    // set the expectations for all other downloads.
    auto index = 0;
    foreach(auto down, downs) {
        EXPECT_CALL(*down, isValid())
            .Times(1)
            .WillRepeatedly(Return(true));
        auto path = QString("local_file %1").arg(index);
        EXPECT_CALL(*down, filePath())
            .Times(1)
            .WillRepeatedly(Return(path));
        EXPECT_CALL(*down, state())
            .Times(1)
            .WillOnce(Return(Download::IDLE));
        EXPECT_CALL(*down, resumeTransfer())
            .Times(0);
        index++;
    }

    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    group->resumeTransfer();

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testStartNoDownloads() {
    QList<GroupDownloadStruct> downloads;

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloads, _algo, _isGSMDownloadAllowed,
        _metadata, _headers, _factory, _fileManager));
    group->startTransfer();
}

void
TestGroupDownload::testStartAllDownloads() {
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);
    auto path = QString("downloadedPath");
    auto second = new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), _metadata, _headers);
    auto third = new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), _metadata, _headers);
    QList<MockDownload*> downs;
    downs.append(first);
    downs.append(second);
    downs.append(third);

    // set the expectations

    EXPECT_CALL(*_factory, createDownloadForGroup(_, _, _, _, _))
        .Times(3)
        .WillOnce(Return(first))
        .WillOnce(Return(second))
        .WillOnce(Return(third));

    // set the expectations for all other downloads.
    auto index = 0;
    foreach(auto down, downs) {
        EXPECT_CALL(*down, isValid())
            .Times(1)
            .WillRepeatedly(Return(true));
        auto path = QString("local_file %1").arg(index);
        EXPECT_CALL(*down, filePath())
            .Times(1)
            .WillRepeatedly(Return(path));
        EXPECT_CALL(*down, state())
            .Times(1)
            .WillOnce(Return(Download::IDLE));
        EXPECT_CALL(*down, startTransfer())
            .Times(1);
        index++;
    }

    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    group->startTransfer();

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testStartAlreadyStarted() {
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);
    auto path = QString("downloadedPath");
    auto second = new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), _metadata, _headers);
    auto third = new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), _metadata, _headers);
    QList<MockDownload*> downs;
    downs.append(first);
    downs.append(second);
    downs.append(third);

    // set the expectations

    EXPECT_CALL(*_factory, createDownloadForGroup(_, _, _, _, _))
        .Times(3)
        .WillOnce(Return(first))
        .WillOnce(Return(second))
        .WillOnce(Return(third));

    // set the expectations for all other downloads.
    auto index = 0;
    foreach(auto down, downs) {
        EXPECT_CALL(*down, isValid())
            .Times(1)
            .WillRepeatedly(Return(true));
        auto path = QString("local_file %1").arg(index);
        EXPECT_CALL(*down, filePath())
            .Times(1)
            .WillRepeatedly(Return(path));
        EXPECT_CALL(*down, state())
            .Times(1)
            .WillOnce(Return(Download::START));
        EXPECT_CALL(*down, startTransfer())
            .Times(0);
        index++;
    }

    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    group->startTransfer();

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testStartFinished() {
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);
    auto path = QString("downloadedPath");
    auto second = new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), _metadata, _headers);
    auto third = new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), _metadata, _headers);
    QList<MockDownload*> downs;
    downs.append(first);
    downs.append(second);
    downs.append(third);

    // set the expectations

    EXPECT_CALL(*_factory, createDownloadForGroup(_, _, _, _, _))
        .Times(3)
        .WillOnce(Return(first))
        .WillOnce(Return(second))
        .WillOnce(Return(third));

    for(auto index = 1; index < downs.count(); index++) {
        EXPECT_CALL(*downs[index], isValid())
            .Times(1)
            .WillRepeatedly(Return(true));
        auto path = QString("local_file %1").arg(index);
        EXPECT_CALL(*downs[index], filePath())
            .Times(1)
            .WillRepeatedly(Return(path));
        EXPECT_CALL(*downs[index], state())
            .Times(1)
            .WillOnce(Return(Download::IDLE));
        EXPECT_CALL(*downs[index], startTransfer())
            .Times(1);
    }

    EXPECT_CALL(*downs[0], isValid())
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*downs[0], filePath())
        .Times(1)
        .WillRepeatedly(Return(QString("path")));
    EXPECT_CALL(*downs[0], state())
        .Times(1)
        .WillOnce(Return(Download::FINISH));
    EXPECT_CALL(*downs[0], startTransfer())
        .Times(0);

    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    group->startTransfer();

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testStartCancel() {
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);
    auto path = QString("downloadedPath");
    auto second = new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), _metadata, _headers);
    auto third = new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), _metadata, _headers);
    QList<MockDownload*> downs;
    downs.append(first);
    downs.append(second);
    downs.append(third);

    // set the expectations

    EXPECT_CALL(*_factory, createDownloadForGroup(_, _, _, _, _))
        .Times(3)
        .WillOnce(Return(first))
        .WillOnce(Return(second))
        .WillOnce(Return(third));

    for(auto index = 1; index < downs.count(); index++) {
        EXPECT_CALL(*downs[index], isValid())
            .Times(1)
            .WillRepeatedly(Return(true));
        auto path = QString("local_file %1").arg(index);
        EXPECT_CALL(*downs[index], filePath())
            .Times(1)
            .WillRepeatedly(Return(path));
        EXPECT_CALL(*downs[index], state())
            .Times(1)
            .WillOnce(Return(Download::IDLE));
        EXPECT_CALL(*downs[index], startTransfer())
            .Times(1);
    }

    EXPECT_CALL(*downs[0], isValid())
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*downs[0], filePath())
        .Times(1)
        .WillRepeatedly(Return(QString("path")));
    EXPECT_CALL(*downs[0], state())
        .Times(1)
        .WillOnce(Return(Download::CANCEL));
    EXPECT_CALL(*downs[0], startTransfer())
        .Times(0);

    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    group->startTransfer();

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testAllDownloadsFinished() {
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);
    auto path = QString("downloadedPath");
    auto second = new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), _metadata, _headers);
    auto third = new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), _metadata, _headers);
    QList<MockDownload*> downs;
    downs.append(first);
    downs.append(second);
    downs.append(third);

    // set the expectations

    EXPECT_CALL(*_factory, createDownloadForGroup(_, _, _, _, _))
        .Times(3)
        .WillOnce(Return(first))
        .WillOnce(Return(second))
        .WillOnce(Return(third));

    // set the expectations for all other downloads.
    auto index = 0;
    foreach(auto down, downs) {
        EXPECT_CALL(*down, isValid())
            .Times(1)
            .WillRepeatedly(Return(true));
        auto path = QString("local_file %1").arg(index);
        EXPECT_CALL(*down, filePath())
            .Times(1)
            .WillRepeatedly(Return(path));
        index++;
    }

    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    SignalBarrier spy(group.data(), SIGNAL(finished(QStringList)));

    // emit signals for each download
    QStringList expectedPaths;
    index = 0;
    foreach(MockDownload* down, downs) {
        auto path = QString("local_path %1").arg(index);
        expectedPaths.append(path);
        down->finished(path);
        index++;
    }

    QVERIFY(spy.ensureSignalEmitted());
    QCOMPARE(spy.count(), 1);

    // ensure that we have the correct paths
    auto arguments = spy.takeFirst();
    auto emittedPaths = arguments.at(0).value<QStringList>();
    foreach(auto path, expectedPaths) {
        QVERIFY(emittedPaths.contains(path));
    }

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testSingleDownloadErrorNoFinished() {
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);
    auto path = QString("downloadedPath");
    auto second = new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), _metadata, _headers);
    auto third = new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), _metadata, _headers);
    QList<MockDownload*> downs;
    downs.append(first);
    downs.append(second);
    downs.append(third);

    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        "my_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    // set the expectations

    EXPECT_CALL(*_factory, createDownloadForGroup(_, _, _, _, _))
        .Times(3)
        .WillOnce(Return(first))
        .WillOnce(Return(second))
        .WillOnce(Return(third));

    // set the expectations for all other downloads.
    for(auto index = 1; index < downs.count(); index ++) {
        EXPECT_CALL(*downs[index], isValid())
            .Times(1)
            .WillRepeatedly(Return(true));
        auto path = QString("local_file %1").arg(index);
        EXPECT_CALL(*downs[index], filePath())
            .Times(1)
            .WillRepeatedly(Return(path));
        EXPECT_CALL(*downs[index], state())
            .Times(1)
            .WillOnce(Return(Download::START));
        EXPECT_CALL(*downs[index], cancelTransfer())
            .Times(1);
    }

    // we are going to tell first to be canceled manually
    EXPECT_CALL(*downs[0], isValid())
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*downs[0], filePath())
        .Times(1)
        .WillRepeatedly(Return(QString("local_file")));
    EXPECT_CALL(*downs[0], state())
        .Times(1)
        .WillOnce(Return(Download::ERROR));

    EXPECT_CALL(*_fileManager, remove(path))
        .Times(0);

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    QSignalSpy spy(group.data(), SIGNAL(error(QString)));
    QSignalSpy canceledSpy(group.data(), SIGNAL(canceled(bool)));
    first->error("testing");

    QCOMPARE(1, spy.count());
    QCOMPARE(0, canceledSpy.count());

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testSingleDownloadErrorWithFinished() {
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);
    auto path = QString("downloadedPath");
    auto second = new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), _metadata, _headers);
    auto third = new MockDownload("", "", "", "",
        QUrl("http://reddit.com"), _metadata, _headers);
    auto finishedPath = QString("finishedPath");
    QList<MockDownload*> downs;
    downs.append(first);
    downs.append(second);
    downs.append(third);

    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        "my_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    // set the expectations

    EXPECT_CALL(*_factory, createDownloadForGroup(_, _, _, _, _))
        .Times(3)
        .WillOnce(Return(first))
        .WillOnce(Return(second))
        .WillOnce(Return(third));

    // we are going to tell first to be canceled manually
    EXPECT_CALL(*first, isValid())
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*first, filePath())
        .Times(1)
        .WillRepeatedly(Return(QString("local_file")));
    EXPECT_CALL(*first, state())
        .Times(1)
        .WillOnce(Return(Download::ERROR));

    // set the expectations for all not finished downloads
    EXPECT_CALL(*second, isValid())
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*second, filePath())
        .Times(1)
        .WillRepeatedly(Return(QString("local_file 2")));
    EXPECT_CALL(*second, state())
        .Times(1)
        .WillOnce(Return(Download::START));
    EXPECT_CALL(*second, cancelTransfer())
        .Times(1);

    // we are going to tell the last download to finish
    EXPECT_CALL(*third, isValid())
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*third, filePath())
        .Times(1)
        .WillRepeatedly(Return(finishedPath));
    EXPECT_CALL(*third, state())
        .Times(1)
        .WillOnce(Return(Download::FINISH));
    EXPECT_CALL(*third, cancelTransfer())
        .Times(0);

    EXPECT_CALL(*_fileManager, remove(finishedPath))
        .Times(1);

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    QSignalSpy spy(group.data(), SIGNAL(error(QString)));
    QSignalSpy canceledSpy(group.data(), SIGNAL(canceled(bool)));
    third->finished(finishedPath);
    first->error("testing");

    QCOMPARE(1, spy.count());
    QCOMPARE(0, canceledSpy.count());

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testLocalPathSingleDownload() {
    auto localFile = QString("local_file");
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);

    // set the expectations
    QPair<QString, QVariant> expected;
    expected.first = Metadata::LOCAL_PATH_KEY;
    expected.second = QVariant(localFile);
    EXPECT_CALL(*_factory,
        createDownloadForGroup(_, _, _, QVariantMapContains(expected), _))
            .Times(1)
            .WillOnce(Return(first));

    // set the expectations for all other downloads.
    EXPECT_CALL(*first, isValid())
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*first, filePath())
        .Times(1)
        .WillRepeatedly(Return(localFile));

    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        localFile, ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    QVERIFY(Mock::VerifyAndClearExpectations(first));
    verifyMocks();
}

void
TestGroupDownload::testConfinedSingleDownload_data() {
    QTest::addColumn<bool>("confined");

    QTest::newRow("Confined") << true;
    QTest::newRow("Unconfined") << false;
}

void
TestGroupDownload::testConfinedSingleDownload() {
    // assert that the created downloads are confined
    QFETCH(bool, confined);
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);

    // set the expectations
    EXPECT_CALL(*_factory,
        createDownloadForGroup(confined, _, _, _, _))
            .Times(1)
            .WillOnce(Return(first));

    // set the expectations for all other downloads.
    EXPECT_CALL(*first, isValid())
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*first, filePath())
        .Times(1)
        .WillRepeatedly(Return("localFile"));

    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        "localFile", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        confined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    QVERIFY(Mock::VerifyAndClearExpectations(first));
    verifyMocks();
}

void
TestGroupDownload::testInvalidDownload() {
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);

    // set the expectations
    EXPECT_CALL(*_factory,
        createDownloadForGroup(_, _, _, _, _))
            .Times(1)
            .WillOnce(Return(first));

    // set the expectations for all other downloads.
    EXPECT_CALL(*first, isValid())
        .Times(1)
        .WillRepeatedly(Return(false));
    EXPECT_CALL(*first, filePath())
        .Times(1)
        .WillRepeatedly(Return("localFile"));

    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        "localFile", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    QVERIFY(Mock::VerifyAndClearExpectations(first));
    verifyMocks();
    QVERIFY(!group->isValid());
}

void
TestGroupDownload::testInvalidHashAlgorithm() {
    // set the expectations
    EXPECT_CALL(*_factory,
        createDownloadForGroup(_, _, _, _, _))
            .Times(0);

    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        "localFile", "signature-goes-here"));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        false, _rootPath, downloadsStruct, "wrong", _isGSMDownloadAllowed, 
        _metadata, _headers, _factory, _fileManager));

    verifyMocks();
    QVERIFY(!group->isValid());
}

void
TestGroupDownload::testValidHashAlgorithm_data() {
    QTest::addColumn<QString>("algo");

    QTest::newRow("md5") << "md5";
    QTest::newRow("sha1") << "sha1";
    QTest::newRow("sha224") << "sha224";
    QTest::newRow("sha256") << "sha256";
    QTest::newRow("sha384") << "sha384";
    QTest::newRow("sha512") << "sha512";
    QTest::newRow("Empty string") << "";
}

void
TestGroupDownload::testValidHashAlgorithm() {
    QFETCH(QString, algo);

    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);

    // set the expectations
    EXPECT_CALL(*_factory,
        createDownloadForGroup(_, _, _, _, _))
            .Times(1)
            .WillOnce(Return(first));

    // set the expectations for all other downloads.
    EXPECT_CALL(*first, isValid())
        .Times(1)
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*first, filePath())
        .Times(1)
        .WillRepeatedly(Return("localFile"));

    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        "localFile", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        false, _rootPath, downloadsStruct, algo, _isGSMDownloadAllowed, 
        _metadata, _headers, _factory, _fileManager));

    QVERIFY(Mock::VerifyAndClearExpectations(first));
    verifyMocks();
    QVERIFY(group->isValid());
}

void
TestGroupDownload::testEmptyGroupRaisesFinish() {
    QList<GroupDownloadStruct> downloadsStruct;
    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path, false,
        _rootPath, downloadsStruct, "md5", _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    SignalBarrier startedSpy(group.data(), SIGNAL(started(bool)));
    SignalBarrier finishedSpy(group.data(), SIGNAL(finished(QStringList)));

    group->startTransfer();
    QVERIFY(startedSpy.ensureSignalEmitted());
    QCOMPARE(startedSpy.count(), 1);
    QVERIFY(finishedSpy.ensureSignalEmitted());
    QCOMPARE(finishedSpy.count(), 1);
}

void
TestGroupDownload::testDuplicatedLocalPath() {
    auto first = new MockDownload("", "", "", "",
        QUrl("http://one.ubunt.com"), _metadata, _headers);
    auto second = new MockDownload("", "", "", "",
        QUrl("http://ubuntu.com"), _metadata, _headers);
    QList<MockDownload*> downs;
    downs.append(first);
    downs.append(second);

    // set the expectations

    EXPECT_CALL(*_factory, createDownloadForGroup(_, _, _, _, _))
        .Times(2)
        .WillOnce(Return(first))
        .WillOnce(Return(second));

    // set the expectations for all other downloads.
    auto path = QString("local_file");
    foreach(auto down, downs) {
        EXPECT_CALL(*down, isValid())
            .Times(AnyNumber())
            .WillRepeatedly(Return(true));
        // ensure that the local path is the same
        EXPECT_CALL(*down, filePath())
            .Times(1)
            .WillRepeatedly(Return(path));
    }

    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        "local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _appId, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
    QVERIFY(!group->isValid());
}


QTEST_MAIN(TestGroupDownload)

