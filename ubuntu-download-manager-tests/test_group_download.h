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

#ifndef TEST_GROUP_DOWNLOAD_H
#define TEST_GROUP_DOWNLOAD_H

#include <QObject>
#include <uuid_factory.h>
#include <group_download.h>
#include "./fake_file_manager.h"
#include "./fake_system_network_info.h"
#include "./fake_request_factory.h"
#include "./fake_process_factory.h"
#include "./fake_download_factory.h"
#include "./test_runner.h"

class TestGroupDownload : public QObject {
    Q_OBJECT

 public:
    explicit TestGroupDownload(QObject *parent = 0);

 private slots:  // NOLINT(whitespace/indent)

    void init();
    void cleanup();

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

 private:
    QUuid _id;
    QString _path;
    QCryptographicHash::Algorithm _algo;
    bool _isGSMDownloadAllowed;
    QVariantMap _metadata;
    QMap<QString, QString> _headers;
    UuidFactory* _uuidFactory;
    FakeSystemNetworkInfo* _networkInfo;
    FakeRequestFactory* _nam;
    FakeProcessFactory* _processFactory;
    FakeDownloadFactory* _downloadFactory;
    FakeFileManager* _fileManager;
};

DECLARE_TEST(TestGroupDownload)
#endif  // TEST_GROUP_DOWNLOAD_H
