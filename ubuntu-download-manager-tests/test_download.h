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

#ifndef TEST_APP_DOWNLOAD_H
#define TEST_APP_DOWNLOAD_H

#include <QDir>
#include <QObject>
#include <ubuntu/downloads/file_download.h>
#include <ubuntu/download_manager/metatypes.h>
#include <ubuntu/download_manager/tests/server/file_manager.h>
#include <ubuntu/download_manager/tests/server/filename_mutex.h>
#include <ubuntu/download_manager/tests/server/system_network_info.h>
#include <ubuntu/download_manager/tests/server/request_factory.h>
#include <ubuntu/download_manager/tests/server/process_factory.h>
#include <ubuntu/download_manager/tests/base_testcase.h>
#include <ubuntu/download_manager/tests/test_runner.h>

class TestDownload: public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestDownload(QObject *parent = 0);

 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;

    // constructors tests
    void testNoHashConstructor();
    void testHashConstructor();

    // data function to be used for the accessor tests
    void testNoHashConstructor_data();
    void testHashConstructor_data();
    void testPath_data();
    void testUrl_data();
    void testProgress_data();
    void testProgressNotKnownSize_data();
    void testOnSuccessHash_data();
    void testSetRawHeadersStart_data();
    void testSetRawHeadersResume_data();
    void testSetRawHeadersWithRangeStart_data();
    void testSetRawHeadersWithRangeResume_data();
    void testSetThrottleNoReply_data();
    void testSetThrottle_data();
    void testSetGSMDownloadSame_data();
    void testSetGSMDownloadDiff_data();
    void testCanDownloadGSM_data();
    void testCanDownloadNoGSM_data();
    void testProcessExecutedNoParams_data();
    void testProcessExecutedWithParams_data();
    void testProcessExecutedWithParamsFile_data();

    // accessor methods
    void testPath();
    void testUrl();
    void testProgress();
    void testProgressNotKnownSize();
    void testTotalSize();
    void testTotalSizeNoProgress();
    void testSetThrottleNoReply();
    void testSetThrottle();
    void testSetGSMDownloadSame();
    void testSetGSMDownloadDiff();
    void testCanDownloadGSM();
    void testCanDownloadNoGSM();

    // dbus method tests
    void testCancel();
    void testPause();
    void testResume();
    void testStart();

    // network related tests
    void testCancelDownload();
    void testCancelDownloadNotStarted();
    void testPauseDownload();
    void testPauseDownloadNotStarted();
    void testResumeRunning();
    void testResumeDownload();
    void testStartDownload();
    void testStartDownloadAlreadyStarted();
    void testOnSuccessNoHash();
    void testOnSuccessHashError();
    void testOnSuccessHash();
    void testOnHttpError_data();
    void testOnHttpError();
    void testOnSslError();
    void testOnNetworkError_data();
    void testOnNetworkError();
    void testOnAuthError();
    void testOnProxyAuthError();
    void testSetRawHeadersStart();
    void testSetRawHeadersWithRangeStart();
    void testSetRawHeadersResume();
    void testSetRawHeadersWithRangeResume();

    // process related tests
    void testProcessExecutedNoParams();
    void testProcessExecutedWithParams();
    void testProcessExecutedWithParamsFile();
    void testProcessFinishedNoError();
    void testProcessFinishedWithError();
    void testProcessError_data();
    void testProcessError();
    void testProcessFinishedCrash();
    void testFileRemoveAfterSuccessfulProcess();

    // test related to bug #1224678
    void testSetRawHeaderAcceptEncoding_data();
    void testSetRawHeaderAcceptEncoding();

    // local path generation tests
    void testLocalPathConfined();
    void testLocalPathNotConfined();

    // ssl error management
    void testSslErrorsIgnored();
    void testSslErrorsNotIgnored();

    // isValid tests
    void testInvalidUrl();
    void testValidUrl();
    void testInvalidHashAlgorithm();
    void testValidHashAlgorithm_data();
    void testValidHashAlgorithm();
    void testInvalidFilePresent();
    void testValidFileNotPresent();

    // filename tests
    void testDownloadPresent();
    void testDownloadPresentSeveralFiles_data();
    void testDownloadPresentSeveralFiles();

    // processing signal tests
    void testProcessingJustOnce();

    // ensure that if we cannot handle the file system
    // we fwd the error
    void testFileSystemErrorProgress();
    void testFileSystemErrorPause();

    // test different redirects
    void testRedirectCycle();
    void testSingleRedirect();
    void testSeveralRedirects_data();
    void testSeveralRedirects();

    // test lock of files
    void testRedirectDoesNotUnlockPath();
    void testCancelUnlocksPath();
    void testFinishUnlocksPath();
    void testProcessFinishUnlocksPath();
    void testErrorUnlocksPath();
    void testCancelUnlocksPathFromMetadata();
    void testFinishUnlocksPathFromMetadata();
    void testProcessFinishUnlocksPathFromMetadata();
    void testErrorUnlocksPathFromMetadata();

 private:
    QString _id;
    bool _isConfined;
    QString _rootPath;
    QVariantMap _metadata;
    QMap<QString, QString> _headers;
    QString _path;
    QUrl _url;
    QString _algo;
    FakeSystemNetworkInfo* _networkInfo;
    FakeRequestFactory* _reqFactory;
    FakeProcessFactory* _processFactory;
    FakeFileManager* _fileManager;
    FakeFileNameMutex* _fileNameMutex;
};

Q_DECLARE_METATYPE(QNetworkInfo::NetworkMode)

#endif  // TEST_APP_DOWNLOAD_H
