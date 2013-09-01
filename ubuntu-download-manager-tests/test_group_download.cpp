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
#include <group_download.h>
#include "./fake_download.h"
#include "./test_group_download.h"

TestGroupDownload::TestGroupDownload(QObject *parent)
    : QObject(parent) {
}

void
TestGroupDownload::init() {
    _id = QUuid::createUuid();
    _path = "/group/dbus/path";
    _algo = QCryptographicHash::Md5;
    _isGSMDownloadAllowed = true;
    _networkInfo = new FakeSystemNetworkInfo();
    _nam = new FakeRequestFactory();
    _processFactory = new FakeProcessFactory();
    _uuidFactory = new UuidFactory();
    _downloadFactory = new FakeDownloadFactory(_uuidFactory, _networkInfo,
        _nam, _processFactory);
    _fileManager = new FakeFileManager();
}

void
TestGroupDownload::cleanup() {
    if (_networkInfo != NULL)
        delete _networkInfo;
    if (_nam != NULL)
        delete _nam;
    if (_processFactory != NULL)
        delete _processFactory;
    if (_downloadFactory != NULL)
        delete _downloadFactory;
}

void
TestGroupDownload::testCancelNoDownloads() {
    QList<GroupDownloadStruct> downloads;

    GroupDownload* group = new GroupDownload(_id, _path, downloads, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo, _nam,
        _processFactory, _fileManager);
    group->cancelDownload();
}

void
TestGroupDownload::testCancelAllDownloads() {
    QList<GroupDownloadStruct> downloads;
    downloads.append(GroupDownloadStruct("http://one.ubunt.com",
        "local_file", ""));
    downloads.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));

    GroupDownload* group = new GroupDownload(_id, _path, downloads, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);
    group->cancelDownload();
    foreach(Download* download, _downloadFactory->downloads()) {
        QCOMPARE(Download::CANCEL, download->state());
    }
}

void
TestGroupDownload::testCancelDownloadWithFinished() {
    _fileManager->record();
    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    GroupDownload* group = new GroupDownload(_id, _path, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);

    QList<Download*> downloads = _downloadFactory->downloads();
    reinterpret_cast<FakeDownload*>(downloads[0])->emitFinished(deleteFile);
    group->cancelDownload();

    foreach(Download* download, downloads) {
        Download::State state = download->state();
        if (state != Download::FINISH)
            QCOMPARE(Download::CANCEL, download->state());
    }
    // assert that remove was indeed called with the correct path
    QList<MethodData> calledMethods = _fileManager->calledMethods();
    qDebug() << "Called methods" << calledMethods;
    StringWrapper* path = reinterpret_cast<StringWrapper*>(
        calledMethods[0].params().inParams()[0]);
    QCOMPARE(deleteFile, path->value());
}

void
TestGroupDownload::testCancelDownloadWithCancel() {
    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    GroupDownload* group = new GroupDownload(_id, _path, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);

    QList<Download*> downloads = _downloadFactory->downloads();
    reinterpret_cast<FakeDownload*>(downloads[0])->cancel();
    group->cancelDownload();

    foreach(Download* download, downloads) {
        Download::State state = download->state();
        if (state != Download::FINISH)
            QCOMPARE(Download::CANCEL, download->state());
    }
}

void
TestGroupDownload::testPauseNoDownloads() {
    QList<GroupDownloadStruct> downloads;

    GroupDownload* group = new GroupDownload(_id, _path, downloads, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo, _nam,
        _processFactory, _fileManager);
    group->pauseDownload();
}

void
TestGroupDownload::testPauseAllDownloads() {
    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        "local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));

    GroupDownload* group = new GroupDownload(_id, _path, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);
    // start all downlaods
    QList<Download*> downloads = _downloadFactory->downloads();
    foreach(Download* download, downloads) {
        download->start();
    }
    group->pauseDownload();
    foreach(Download* download, downloads) {
        QCOMPARE(Download::PAUSE, download->state());
    }
}

void
TestGroupDownload::testPauseDownloadWithFinished() {
    _fileManager->record();
    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    GroupDownload* group = new GroupDownload(_id, _path, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);

    QList<Download*> downloads = _downloadFactory->downloads();
    reinterpret_cast<FakeDownload*>(downloads[0])->emitFinished(deleteFile);
    group->pauseDownload();

    foreach(Download* download, downloads) {
        Download::State state = download->state();
        if (state != Download::FINISH && state != Download::IDLE)
            QCOMPARE(Download::PAUSE, download->state());
    }
    // assert that remove was indeed called with the correct path
    QList<MethodData> calledMethods = _fileManager->calledMethods();
    QCOMPARE(0, calledMethods.count());
    qDebug() << Download::FINISH << downloads[0]->state();
    QCOMPARE(Download::FINISH, downloads[0]->state());
}

void
TestGroupDownload::testPauseDownloadWithCancel() {
    _fileManager->record();
    QList<GroupDownloadStruct> downloadsStruct;
    QString deleteFile = "local_file";
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        deleteFile, ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://reddit.com",
        "other_reddit_local_file", ""));

    GroupDownload* group = new GroupDownload(_id, _path, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);

    QList<Download*> downloads = _downloadFactory->downloads();
    reinterpret_cast<FakeDownload*>(downloads[0])->cancel();
    group->pauseDownload();

    foreach(Download* download, downloads) {
        Download::State state = download->state();
        if (state != Download::CANCEL && state != Download::IDLE)
            QCOMPARE(Download::PAUSE, state);
    }
    QList<MethodData> calledMethods = _fileManager->calledMethods();
    QCOMPARE(0, calledMethods.count());
}

void
TestGroupDownload::testResumeNoDownloads() {
    QList<GroupDownloadStruct> downloads;

    GroupDownload* group = new GroupDownload(_id, _path, downloads, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo, _nam,
        _processFactory, _fileManager);
    group->cancelDownload();
}

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

    GroupDownload* group = new GroupDownload(_id, _path, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);

    QList<Download*> downloads = _downloadFactory->downloads();
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

    GroupDownload* group = new GroupDownload(_id, _path, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);

    QList<Download*> downloads = _downloadFactory->downloads();
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

    GroupDownload* group = new GroupDownload(_id, _path, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);

    QList<Download*> downloads = _downloadFactory->downloads();
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

    GroupDownload* group = new GroupDownload(_id, _path, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);

    QList<Download*> downloads = _downloadFactory->downloads();
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

    GroupDownload* group = new GroupDownload(_id, _path, downloads, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo, _nam,
        _processFactory, _fileManager);
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

    GroupDownload* group = new GroupDownload(_id, _path, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);

    QList<Download*> downloads = _downloadFactory->downloads();
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

    GroupDownload* group = new GroupDownload(_id, _path, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);

    QList<Download*> downloads = _downloadFactory->downloads();
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

    GroupDownload* group = new GroupDownload(_id, _path, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);

    QList<Download*> downloads = _downloadFactory->downloads();
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

    GroupDownload* group = new GroupDownload(_id, _path, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);

    QList<Download*> downloads = _downloadFactory->downloads();
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

    GroupDownload* group = new GroupDownload(_id, _path, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);

    QList<Download*> downloads = _downloadFactory->downloads();
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

    GroupDownload* group = new GroupDownload(_id, _path, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);
    QSignalSpy spy(group, SIGNAL(finished(QStringList)));

    QList<Download*> downloads = _downloadFactory->downloads();
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

    GroupDownload* group = new GroupDownload(_id, _path, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);
    QSignalSpy spy(group, SIGNAL(finished(QStringList)));

    QList<Download*> downloads = _downloadFactory->downloads();
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

    GroupDownload* group = new GroupDownload(_id, _path, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);
    QSignalSpy spy(group, SIGNAL(error(QString)));

    QList<Download*> downloads = _downloadFactory->downloads();
    group->startDownload();

    reinterpret_cast<FakeDownload*>(downloads[0])->emitError("error");

    QList<MethodData> calledMethods = _fileManager->calledMethods();
    QCOMPARE(0, calledMethods.count());
    QCOMPARE(spy.count(), 1);
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

    GroupDownload* group = new GroupDownload(_id, _path, downloadsStruct, _algo,
        _isGSMDownloadAllowed, _metadata, _headers, _networkInfo,
        _downloadFactory, _fileManager);
    QSignalSpy spy(group, SIGNAL(error(QString)));

    QList<Download*> downloads = _downloadFactory->downloads();
    group->startDownload();
    for (int index = 1; index < downloads.count(); index++) {
        reinterpret_cast<FakeDownload*>(downloads[index])->emitFinished("path");
    }

    reinterpret_cast<FakeDownload*>(downloads[0])->emitError("error");

    QList<MethodData> calledMethods = _fileManager->calledMethods();
    QCOMPARE(2, calledMethods.count());
    QCOMPARE(spy.count(), 1);
}
