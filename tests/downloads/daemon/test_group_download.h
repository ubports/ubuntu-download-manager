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
#include "base_testcase.h"
#include "factory.h"
#include "file_manager.h"
#include "process_factory.h"
#include "request_factory.h"
#include "system_network_info.h"

using namespace Ubuntu::DownloadManager;
using namespace Ubuntu::DownloadManager::Daemon;
using namespace Ubuntu::Transfers::Tests;

class TestGroupDownload : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestGroupDownload(QObject *parent = 0)
        : BaseTestCase("TestGroupDownload", parent) { }

private:
    void verifyMocks();

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
    void testResumeNoStarted();
    void testStartNoDownloads();
    void testStartAllDownloads();
    void testStartAlreadyStarted();
    void testStartFinished();
    void testStartCancel();
    void testAllDownloadsFinished();
    void testSingleDownloadErrorNoFinished();
    void testSingleDownloadErrorWithFinished();
    void testLocalPathSingleDownload();
    void testConfinedSingleDownload_data();
    void testConfinedSingleDownload();

    // isValid tests
    void testInvalidDownload();
    void testInvalidHashAlgorithm();
    void testValidHashAlgorithm_data();
    void testValidHashAlgorithm();

    // empty group tests
    void testEmptyGroupRaisesFinish();

    // ensure that local paths are just used once
    void testDuplicatedLocalPath();


 private:
    QString _id;
    QString _appId;
    QString _path;
    bool _isConfined;
    QString _rootPath;
    QString _algo;
    bool _isGSMDownloadAllowed;
    QVariantMap _metadata;
    QMap<QString, QString> _headers;
    MockSystemNetworkInfo* _networkInfo;
    MockDownloadFactory* _factory;
    MockFileManager* _fileManager;
};

#endif  // TEST_GROUP_DOWNLOAD_H
