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
#include <QSignalSpy>
#include <ubuntu/transfers/metadata.h>
#include <ubuntu/downloads/group_download.h>
#include <ubuntu/transfers/system/uuid_utils.h>
#include "download.h"
#include "test_group_download.h"

using ::testing::_;
using ::testing::Mock;
using ::testing::AnyNumber;
using ::testing::Return;

void
TestGroupDownload::init() {
    BaseTestCase::init();
    _id = UuidUtils::getDBusString(QUuid::createUuid());
    _path = "/group/dbus/path";
    _isConfined = false;
    _rootPath = "/random/dbus/path";
    _algo = "Md5";
    _isGSMDownloadAllowed = true;
    _factory = new MockDownloadFactory();
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

    SystemNetworkInfo::deleteInstance();
    FileManager::deleteInstance();
    delete _factory;
}

void
TestGroupDownload::testCancelNoDownloads() {
    QList<GroupDownloadStruct> downloads;

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined,
        _rootPath, downloads, _algo, _isGSMDownloadAllowed, _metadata,
        _headers, _factory, _fileManager);
    group->cancelDownload();
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
        EXPECT_CALL(*down, cancelDownload())
            .Times(1);
        index++;
    }

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path,
        _isConfined, _rootPath, downloads, _algo, _isGSMDownloadAllowed,
        _metadata, _headers, _factory, _fileManager));
    group->cancelDownload();

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
        EXPECT_CALL(*down, cancelDownload())
            .Times(1);
        index++;
    }

    auto downloadedPath = QString("downloaded");
    // we expect the downloadedPath to be removed from the fs

    EXPECT_CALL(*_fileManager, remove(downloadedPath))
        .Times(1)
        .WillOnce(Return(true));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    first->finished(downloadedPath);
    group->cancelDownload();

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
        EXPECT_CALL(*down, cancelDownload())
            .Times(1);
        index++;
    }

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    // cancel a download and the expectations will make sue that we are
    // only calling cancelDownload once
    first->setAddToQueue(false);  // so that we call cancelDownload directly
    first->cancel();
    group->cancelDownload();

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testPauseNoDownloads() {
    QList<GroupDownloadStruct> downloads;

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, _isConfined,
        _rootPath, downloads, _algo, _isGSMDownloadAllowed, _metadata,
        _headers, _factory, _fileManager));
    group->pauseDownload();
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
        EXPECT_CALL(*down, startDownload())
            .Times(1);
        EXPECT_CALL(*down, state())
            .Times(1)
            .WillOnce(Return(Download::START));
        EXPECT_CALL(*down, pauseDownload())
            .Times(1);
        index++;
    }

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));
    group->startDownload();

    // trust mocks to ensure that pauseDownload was called in all downloads
    group->pauseDownload();

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
    EXPECT_CALL(*first, startDownload())
        .Times(1);
    EXPECT_CALL(*first, state())
        .Times(1)
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
        EXPECT_CALL(*down, startDownload())
            .Times(1);
        EXPECT_CALL(*down, state())
            .Times(1)
            .WillOnce(Return(Download::START));
        EXPECT_CALL(*down, pauseDownload())
            .Times(1);
        index++;
    }

    EXPECT_CALL(*_fileManager, remove(path))
        .Times(0);

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    group->startDownload();
    first->finished(path);
    group->pauseDownload();

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
        .Times(AnyNumber())
        .WillOnce(Return(first))
        .WillOnce(Return(second))
        .WillOnce(Return(third));

    // set the expectations for all other downloads.
    auto index = 0;
    foreach(MockDownload* down, downs) {
        EXPECT_CALL(*down, isValid())
            .Times(1)
            .WillRepeatedly(Return(true));
        EXPECT_CALL(*down, filePath())
            .Times(1)
            .WillRepeatedly(Return(QString("local_file %1").arg(index)));
        EXPECT_CALL(*down, startDownload())
            .Times(1);
        EXPECT_CALL(*down, pauseDownload())
            .Times(0);
        EXPECT_CALL(*down, cancelDownload())
            .Times(1);
        index++;
    }

    // we are going to tell first to be canceled manually
    EXPECT_CALL(*first, state())
        .Times(1)
        .WillOnce(Return(Download::CANCEL));

    EXPECT_CALL(*_fileManager, remove(path))
        .Times(0);

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    group->startDownload();
    first->cancelDownload();
    first->canceled(true);
    group->pauseDownload();

    foreach(MockDownload* down, downs) {
        QVERIFY(Mock::VerifyAndClearExpectations(down));
    }
    verifyMocks();
}

void
TestGroupDownload::testResumeNoDownloads() {
    QList<GroupDownloadStruct> downloads;

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path,
        _isConfined, _rootPath, downloads, _algo, _isGSMDownloadAllowed,
        _metadata, _headers, _factory, _fileManager));
    group->cancelDownload();
    verifyMocks();
}

/*
void
TestGroupDownload::testResumeAllDownloads() {
    _fileManager->record();
    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path,
        _isConfined, _rootPath, downloadsStruct, _algo, _isGSMDownloadAllowed,
        _metadata, _headers, _factory, _fileManager));

    QList<Download*> downloads = _factory->downloads();
    foreach(Download* download, downloads) {
        download->start();
        download->pause();
    }
    group->resumeDownload();

    foreach(Download* download, downloads) {
        QCOMPARE(Download::RESUME, download->state());
    }
    QList<MethodData> calledMethods = _fileManager->calledMethods();
    QCOMPARE(0, calledMethods.count());
}

void
TestGroupDownload::testResumeWithFinished() {
    _fileManager->record();
    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path,
        _isConfined, _rootPath, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _factory,
        _fileManager));

    QList<Download*> downloads = _factory->downloads();
    qDebug() << "Downloads" << downloads;
    reinterpret_cast<FakeDownload*>(downloads[0])->emitFinished(deleteFile);
    for (int index = 1; index < downloads.count(); index++) {
        downloads[index]->start();
        downloads[index]->pause();
    }
    group->resumeDownload();

    foreach(Download* download, downloads) {
        Download::State state = download->state();
        if (state != Download::FINISH)
            QCOMPARE(Download::RESUME, download->state());
    }
    QList<MethodData> calledMethods = _fileManager->calledMethods();
    QCOMPARE(0, calledMethods.count());
}

void
TestGroupDownload::testResumeWidhCancel() {
    _fileManager->record();
    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, _isConfined,
        _rootPath, downloadsStruct, _algo, _isGSMDownloadAllowed,
        _metadata, _headers, _factory, _fileManager));

    QList<Download*> downloads = _factory->downloads();
    (downloads[0])->cancel();
    for (int index = 1; index < downloads.count(); index++) {
        downloads[index]->start();
        downloads[index]->pause();
    }
    group->resumeDownload();

    foreach(Download* download, downloads) {
        Download::State state = download->state();
        if (state != Download::CANCEL)
            QCOMPARE(Download::RESUME, download->state());
    }
    QList<MethodData> calledMethods = _fileManager->calledMethods();
    QCOMPARE(0, calledMethods.count());
}

void
TestGroupDownload::testReusmeNoStarted() {
    _fileManager->record();
    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, _isConfined,
        _rootPath, downloadsStruct, _algo, _isGSMDownloadAllowed,
        _metadata, _headers, _factory,
        _fileManager));

    QList<Download*> downloads = _factory->downloads();
    group->resumeDownload();

    foreach(Download* download, downloads) {
        QCOMPARE(Download::IDLE, download->state());
    }
    QList<MethodData> calledMethods = _fileManager->calledMethods();
    QCOMPARE(0, calledMethods.count());
}

void
TestGroupDownload::testStartNoDownloads() {
    QList<GroupDownloadStruct> downloads;

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, _isConfined,
        _rootPath, downloads, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));
    group->startDownload();
}

void
TestGroupDownload::testStartAllDownloads() {
    _fileManager->record();
    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    QList<Download*> downloads = _factory->downloads();
    group->startDownload();

    foreach(Download* download, downloads) {
        QCOMPARE(Download::START, download->state());
    }
    QList<MethodData> calledMethods = _fileManager->calledMethods();
    QCOMPARE(0, calledMethods.count());
}

void
TestGroupDownload::testStartAlreadyStarted() {
    _fileManager->record();
    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    QList<Download*> downloads = _factory->downloads();
    group->startDownload();

    foreach(Download* download, downloads) {
        download->start();
    }

    foreach(Download* download, downloads) {
        QCOMPARE(Download::START, download->state());
    }
    QList<MethodData> calledMethods = _fileManager->calledMethods();
    QCOMPARE(0, calledMethods.count());
}

void
TestGroupDownload::testStartResume() {
    _fileManager->record();
    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    QList<Download*> downloads = _factory->downloads();
    group->startDownload();

    foreach(Download* download, downloads) {
        download->start();
        download->pause();
        download->resume();
    }

    foreach(Download* download, downloads) {
        QCOMPARE(Download::RESUME, download->state());
    }
    QList<MethodData> calledMethods = _fileManager->calledMethods();
    QCOMPARE(0, calledMethods.count());
}

void
TestGroupDownload::testStartFinished() {
    _fileManager->record();
    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    QList<Download*> downloads = _factory->downloads();
    reinterpret_cast<FakeDownload*>(downloads[0])->emitFinished(deleteFile);
    group->startDownload();

    foreach(Download* download, downloads) {
        Download::State state = download->state();
        if (state != Download::FINISH)
            QCOMPARE(Download::START, download->state());
    }
    QList<MethodData> calledMethods = _fileManager->calledMethods();
    QCOMPARE(0, calledMethods.count());
}

void
TestGroupDownload::testStartCancel() {
    _fileManager->record();
    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    QList<Download*> downloads = _factory->downloads();
    downloads[0]->cancel();
    group->startDownload();

    foreach(Download* download, downloads) {
        Download::State state = download->state();
        if (state != Download::CANCEL)
            QCOMPARE(Download::START, download->state());
    }
    QList<MethodData> calledMethods = _fileManager->calledMethods();
    QCOMPARE(0, calledMethods.count());
    QCOMPARE(Download::CANCEL, downloads[0]->state());
}

void
TestGroupDownload::testSingleDownloadFinished() {
    _fileManager->record();
    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));
    QSignalSpy spy(group.data(), SIGNAL(finished(QStringList)));

    QList<Download*> downloads = _factory->downloads();
    group->startDownload();

    reinterpret_cast<FakeDownload*>(downloads[0])->emitFinished(deleteFile);

    QList<MethodData> calledMethods = _fileManager->calledMethods();
    QCOMPARE(0, calledMethods.count());
    QCOMPARE(spy.count(), 0);
}

void
TestGroupDownload::testAllDownloadsFinished() {
    _fileManager->record();
    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));
    QSignalSpy spy(group.data(), SIGNAL(finished(QStringList)));

    QList<Download*> downloads = _factory->downloads();
    group->startDownload();
    foreach(Download* download, downloads) {
        reinterpret_cast<FakeDownload*>(download)->emitFinished(deleteFile);
    }

    QList<MethodData> calledMethods = _fileManager->calledMethods();
    QCOMPARE(0, calledMethods.count());
    QCOMPARE(spy.count(), 1);
}

void
TestGroupDownload::testSingleDownloadErrorNoFinished() {
    _fileManager->record();
    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));
    QSignalSpy spy(group.data(), SIGNAL(error(QString)));

    QList<Download*> downloads = _factory->downloads();
    group->startDownload();

    reinterpret_cast<FakeDownload*>(downloads[0])->emitError("error");

    QList<MethodData> calledMethods = _fileManager->calledMethods();
    QCOMPARE(0, calledMethods.count());
    QCOMPARE(spy.count(), 1);
    QCOMPARE(Download::ERROR, group->state());
}

void
TestGroupDownload::testSingleDownloadErrorWithFinished() {
    _fileManager->record();
    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));
    QSignalSpy spy(group.data(), SIGNAL(error(QString)));

    QList<Download*> downloads = _factory->downloads();
    group->startDownload();
    for (int index = 1; index < downloads.count(); index++) {
        reinterpret_cast<FakeDownload*>(downloads[index])->emitFinished("path");
    }

    reinterpret_cast<FakeDownload*>(downloads[0])->emitError("error");

    QList<MethodData> calledMethods = _fileManager->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(spy.count(), 1);
    QCOMPARE(Download::ERROR, group->state());
}

void
TestGroupDownload::testLocalPathSingleDownload() {
    // assert that the local path of the download was set in the metadata
    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        "local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    Q_UNUSED(group);

    QList<Download*> downloads = _factory->downloads();

    // assert that each metadata has the local file set
    QVariantMap downMeta = downloads[0]->metadata();
    QVERIFY(downMeta.contains(Metadata::LOCAL_PATH_KEY));
    QCOMPARE(downMeta[Metadata::LOCAL_PATH_KEY].toString(),
        downloadsStruct[0].getLocalFile());

    downMeta = downloads[1]->metadata();
    QVERIFY(downMeta.contains(Metadata::LOCAL_PATH_KEY));
    QCOMPARE(downMeta[Metadata::LOCAL_PATH_KEY].toString(),
        downloadsStruct[1].getLocalFile());

    downMeta = downloads[2]->metadata();
    QVERIFY(downMeta.contains(Metadata::LOCAL_PATH_KEY));
    QCOMPARE(downMeta[Metadata::LOCAL_PATH_KEY].toString(),
        downloadsStruct[2].getLocalFile());
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

    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        "local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, confined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    Q_UNUSED(group);

    foreach(Download* download, _factory->downloads()) {
        QCOMPARE(confined, download->isConfined());
    }
}

void
TestGroupDownload::testInvalidUrl() {
    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("",
        "local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, false, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    QVERIFY(!group->isValid());
}

void
TestGroupDownload::testValidUrl() {
    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubuntu.com",
        "local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, false, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    QVERIFY(group->isValid());
}

void
TestGroupDownload::testInvalidHashAlgorithm() {
    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubuntu.com",
        "local_file", "asasas"));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", "sasas"));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", "sasaas"));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, false, _rootPath,
        downloadsStruct, "wrong", _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

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
    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubuntu.com",
        "local_file", "asasas"));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", "asasas"));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", "sasa"));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, false, _rootPath,
        downloadsStruct, algo, _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    QVERIFY(group->isValid());
}

void
TestGroupDownload::testInvalidFilePresent() {
    QString filePath = testDirectory() + QDir::separator() + "test_file.jpg";
    QScopedPointer<QFile> file(new QFile(filePath));
    file->open(QIODevice::ReadWrite | QFile::Append);
    file->write("data data data!");
    file->close();

    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubuntu.com",
        "local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        filePath, ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, false, _rootPath,
        downloadsStruct, "md5", _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    QVERIFY(!group->isValid());
}

void
TestGroupDownload::testValidFileNotPresent() {
    QString filePath = testDirectory() + QDir::separator() + "test_file.jpg";

    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubuntu.com",
        "local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        filePath, ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, false, _rootPath,
        downloadsStruct, "md5", _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    QVERIFY(group->isValid());
}

void
TestGroupDownload::testEmptyGroupRaisesFinish() {
    QList<GroupDownloadStruct> downloadsStruct;
    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, false, _rootPath,
        downloadsStruct, "md5", _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    QSignalSpy startedSpy(group.data(), SIGNAL(started(bool)));
    QSignalSpy finishedSpy(group.data(), SIGNAL(finished(QStringList)));

    group->startDownload();
    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 1);
}

void
TestGroupDownload::testDuplicatedLocalPath() {
    QString filePath = testDirectory() + QDir::separator() + "test_file.jpg";

    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubuntu.com",
        filePath, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        filePath, ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, false, _rootPath,
        downloadsStruct, "md5", _isGSMDownloadAllowed, _metadata, _headers,
        _factory, _fileManager));

    QVERIFY(!group->isValid());
}

void
TestGroupDownload::testAuthErrorEmitted_data() {
    QTest::addColumn<QString>("url");

    QTest::newRow("First url") << "http://www.one.ubuntu.com";
    QTest::newRow("Second url") << "http://ubuntu.com/file";
}

void
TestGroupDownload::testAuthErrorEmitted() {
    QFETCH(QString, url);
    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct(url,
        "first path", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "second path", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<FakeGroupDownload> group(new FakeGroupDownload(_id, _path,
        false, _rootPath, downloadsStruct, "md5", _isGSMDownloadAllowed,
        _metadata, _headers, _factory, _fileManager));

    QSignalSpy spy(group.data(), SIGNAL(authError(QString, AuthErrorStruct)));
    group->emitAuthError(url, AuthErrorStruct());
    QCOMPARE(1, spy.count());
}

void
TestGroupDownload::testHttpErrorEmitted_data() {
    QTest::addColumn<QString>("url");

    QTest::newRow("First url") << "http://www.one.ubuntu.com";
    QTest::newRow("Second url") << "http://ubuntu.com/file";
}

void
TestGroupDownload::testHttpErrorEmitted() {
    QFETCH(QString, url);
    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct(url,
        "first path", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "second path", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<FakeGroupDownload> group(new FakeGroupDownload(_id, _path,
        false, _rootPath, downloadsStruct, "md5", _isGSMDownloadAllowed,
        _metadata, _headers, _factory, _fileManager));

    QSignalSpy spy(group.data(), SIGNAL(httpError(QString, HttpErrorStruct)));
    group->emitHttpError(url, HttpErrorStruct());
    QCOMPARE(1, spy.count());
}

void
TestGroupDownload::testNetworkErrorEmitted_data() {
    QTest::addColumn<QString>("url");

    QTest::newRow("First url") << "http://www.one.ubuntu.com";
    QTest::newRow("Second url") << "http://ubuntu.com/file";
}

void
TestGroupDownload::testNetworkErrorEmitted() {
    QFETCH(QString, url);
    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct(url,
        "first path", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "second path", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<FakeGroupDownload> group(new FakeGroupDownload(_id, _path,
        false, _rootPath, downloadsStruct, "md5", _isGSMDownloadAllowed,
        _metadata, _headers, _factory, _fileManager));

    QSignalSpy spy(group.data(), SIGNAL(networkError(QString, NetworkErrorStruct)));
    group->emitNetworkError(url, NetworkErrorStruct());
    QCOMPARE(1, spy.count());
}

void
TestGroupDownload::testProcessErrorEmitted_data() {
    QTest::addColumn<QString>("url");

    QTest::newRow("First url") << "http://www.one.ubuntu.com";
    QTest::newRow("Second url") << "http://ubuntu.com/file";
}

void
TestGroupDownload::testProcessErrorEmitted() {
    QFETCH(QString, url);
    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct(url,
        "first path", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "second path", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    QScopedPointer<FakeGroupDownload> group(new FakeGroupDownload(_id, _path,
        false, _rootPath, downloadsStruct, "md5", _isGSMDownloadAllowed,
        _metadata, _headers, _factory, _fileManager));

    QSignalSpy spy(group.data(), SIGNAL(processError(QString, ProcessErrorStruct)));
    group->emitProcessError(url, ProcessErrorStruct());
    QCOMPARE(1, spy.count());
}
*/

QTEST_MAIN(TestGroupDownload)
