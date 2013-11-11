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

#include <QScopedPointer>
#include <QSignalSpy>
#include <downloads/group_download.h>
#include <system/uuid_utils.h>
#include "fakes/fake_download.h"
#include "test_group_download.h"

TestGroupDownload::TestGroupDownload(QObject *parent)
    : BaseTestCase("TestGroupDownload", parent) {
}

void
TestGroupDownload::init() {
    BaseTestCase::init();
    _id = UuidUtils::getDBusString(QUuid::createUuid());
    _path = "/group/dbus/path";
    _isConfined = true;
    _rootPath = "/random/dbus/path";
    _algo = "Md5";
    _isGSMDownloadAllowed = true;
    _networkInfo = new FakeSystemNetworkInfo();
    _nam = new FakeRequestFactory();
    _processFactory = new FakeProcessFactory();
    _uuidFactory = new UuidFactory();
    _apparmor = new FakeAppArmor(QSharedPointer<UuidFactory>(_uuidFactory));
    _downloadFactory = new FakeDownloadFactory(
        QSharedPointer<AppArmor>(_apparmor),
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<RequestFactory>(_nam),
        QSharedPointer<ProcessFactory>(_processFactory));
    _fileManager = new FakeFileManager();
}

void
TestGroupDownload::cleanup() {
    BaseTestCase::cleanup();

    if (_networkInfo != NULL)
        delete _networkInfo;
    if (_nam != NULL)
        delete _nam;
    if (_processFactory != NULL)
        delete _processFactory;
    if (_downloadFactory != NULL)
        delete _downloadFactory;
    if (_fileManager != NULL)
        delete _fileManager;
}

void
TestGroupDownload::testCancelNoDownloads() {
    QList<GroupDownloadStruct> downloads;

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined,
        _rootPath, downloads, _algo, _isGSMDownloadAllowed, _metadata,
        _headers, QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));
    group->cancelDownload();
}

void
TestGroupDownload::testCancelAllDownloads() {
    QList<GroupDownloadStruct> downloads;
    downloads.append(GroupDownloadStruct("http://one.ubunt.com",
        "local_file", ""));
    downloads.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloads, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));
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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined,
        _rootPath, downloads, _algo, _isGSMDownloadAllowed, _metadata,
        _headers, QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));
    group->pauseDownload();
}

void
TestGroupDownload::testPauseAllDownloads() {
    QList<GroupDownloadStruct> downloadsStruct;
    downloadsStruct.append(GroupDownloadStruct("http://one.ubunt.com",
        "local_file", ""));
    downloadsStruct.append(GroupDownloadStruct("http://ubuntu.com",
        "other_local_file", ""));

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));
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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloads, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));
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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

    QList<Download*> downloads = _downloadFactory->downloads();
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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined,
        _rootPath, downloadsStruct, _algo, _isGSMDownloadAllowed,
        _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined,
        _rootPath, downloadsStruct, _algo, _isGSMDownloadAllowed,
        _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined,
        _rootPath, downloads, _algo,
        _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));
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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));
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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));
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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));
    QSignalSpy spy(group, SIGNAL(error(QString)));

    QList<Download*> downloads = _downloadFactory->downloads();
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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));
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

    GroupDownload* group = new GroupDownload(_id, _path, _isConfined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

    Q_UNUSED(group);

    QList<Download*> downloads = _downloadFactory->downloads();

    // assert that each metadata has the local file set
    QVariantMap downMeta = downloads[0]->metadata();
    QVERIFY(downMeta.contains(LOCAL_PATH_KEY));
    QCOMPARE(downMeta[LOCAL_PATH_KEY].toString(),
        downloadsStruct[0].getLocalFile());

    downMeta = downloads[1]->metadata();
    QVERIFY(downMeta.contains(LOCAL_PATH_KEY));
    QCOMPARE(downMeta[LOCAL_PATH_KEY].toString(),
        downloadsStruct[1].getLocalFile());

    downMeta = downloads[2]->metadata();
    QVERIFY(downMeta.contains(LOCAL_PATH_KEY));
    QCOMPARE(downMeta[LOCAL_PATH_KEY].toString(),
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

    GroupDownload* group = new GroupDownload(_id, _path, confined, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

    Q_UNUSED(group);

    foreach(Download* download, _downloadFactory->downloads()) {
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

    GroupDownload* group = new GroupDownload(_id, _path, false, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

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

    GroupDownload* group = new GroupDownload(_id, _path, false, _rootPath,
        downloadsStruct, _algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

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

    GroupDownload* group = new GroupDownload(_id, _path, false, _rootPath,
        downloadsStruct, "wrong", _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

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

    GroupDownload* group = new GroupDownload(_id, _path, false, _rootPath,
        downloadsStruct, algo, _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

    QVERIFY(group->isValid());
}

void
TestGroupDownload::testInvalidFilePresent() {
    QString filePath = testDirectory() + QDir::separator() + "test_file.jpg";
    QFile* file = new QFile(filePath);
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

    GroupDownload* group = new GroupDownload(_id, _path, false, _rootPath,
        downloadsStruct, "md5", _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

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

    GroupDownload* group = new GroupDownload(_id, _path, false, _rootPath,
        downloadsStruct, "md5", _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager));

    QVERIFY(group->isValid());
}

void
TestGroupDownload::testEmptyGroupRaisesFinish() {
    QList<GroupDownloadStruct> downloadsStruct;
    QScopedPointer<GroupDownload> group(new GroupDownload(_id, _path, false, _rootPath,
        downloadsStruct, "md5", _isGSMDownloadAllowed, _metadata, _headers,
        QSharedPointer<SystemNetworkInfo>(_networkInfo),
        QSharedPointer<Factory>(_downloadFactory),
        QSharedPointer<FileManager>(_fileManager)));

    QSignalSpy startedSpy(group.data(), SIGNAL(started(bool)));
    QSignalSpy finishedSpy(group.data(), SIGNAL(finished(QStringList)));

    group->startDownload();
    QCOMPARE(startedSpy.count(), 1);
    QCOMPARE(finishedSpy.count(), 1);
}
