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

#ifndef TEST_GROUP_DOWNLOAD_H
#define TEST_GROUP_DOWNLOAD_H

#include <QObject>
#include <ubuntu/transfers/system/uuid_factory.h>
#include <ubuntu/downloads/group_download.h>
#include <ubuntu/download_manager/tests/server/apparmor.h>
#include <ubuntu/download_manager/tests/server/file_manager.h>
#include <ubuntu/download_manager/tests/server/system_network_info.h>
#include <ubuntu/download_manager/tests/server/request_factory.h>
#include <ubuntu/download_manager/tests/server/process_factory.h>
#include <ubuntu/download_manager/tests/server/factory.h>
#include <ubuntu/download_manager/tests/base_testcase.h>

using namespace Ubuntu::DownloadManager;
using namespace Ubuntu::DownloadManager::Daemon;

class TestGroupDownload : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestGroupDownload(QObject *parent = 0);

 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;

    void testCancelNoDownloads();
    void testCancelAllDownloads();
    void testCancelDownloadWithFinished();
    void testCancelDownloadWithCancel();
    void testPauseNoDownloads();
    void testPauseAllDownloads();
    void testPauseDownloadWithFinished();
    void testPauseDownloadWithCancel();
    void testResumeNoDownloads();
    void testResumeAllDownloads();
    void testResumeWithFinished();
    void testResumeWidhCancel();
    void testReusmeNoStarted();
    void testStartNoDownloads();
    void testStartAllDownloads();
    void testStartAlreadyStarted();
    void testStartResume();
    void testStartFinished();
    void testStartCancel();
    void testSingleDownloadFinished();
    void testAllDownloadsFinished();
    void testSingleDownloadErrorNoFinished();
    void testSingleDownloadErrorWithFinished();
    void testLocalPathSingleDownload();
    void testConfinedSingleDownload_data();
    void testConfinedSingleDownload();

    // isValid tests
    void testInvalidUrl();
    void testValidUrl();
    void testInvalidHashAlgorithm();
    void testValidHashAlgorithm_data();
    void testValidHashAlgorithm();
    void testInvalidFilePresent();
    void testValidFileNotPresent();

    // empty group tests
    void testEmptyGroupRaisesFinish();

    // ensure that local paths are just used once
    void testDuplicatedLocalPath();

 private:
    QString _id;
    QString _path;
    bool _isConfined;
    QString _rootPath;
    QString _algo;
    bool _isGSMDownloadAllowed;
    QVariantMap _metadata;
    QMap<QString, QString> _headers;
    UuidFactory* _uuidFactory;
    FakeAppArmor* _apparmor;
    FakeSystemNetworkInfo* _networkInfo;
    FakeDownloadFactory* _downloadFactory;
    FakeFileManager* _fileManager;
};

#endif  // TEST_GROUP_DOWNLOAD_H
