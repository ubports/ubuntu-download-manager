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
#include <QTest>
#include <ubuntu/downloads/file_download.h>
#include <ubuntu/download_manager/metatypes.h>
#include <ubuntu/transfers/system/uuid_utils.h>
#include <file_manager.h>
#include <process_factory.h>
#include <request_factory.h>
#include <system_network_info.h>

#include "base_testcase.h"
#include "cryptographic_hash.h"

using namespace Ubuntu::Transfers::System;
using namespace Ubuntu::Transfers::Tests;

class TestDownload: public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestDownload(QObject *parent = 0)
        : BaseTestCase("TestDownload", parent) {}

 private:
    void verifyMocks();

 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;

    // constructors tests
    void testNoHashConstructor();
    void testHashConstructor();
    void testConfinedNoClickMetadata();
    void testUnconfinedWithClickMetadata();

    // data function to be used for the accessor tests
    void testNoHashConstructor_data();
    void testHashConstructor_data();
    void testPath_data();
    void testUrl_data();
    void testProgress_data();
    void testProgressNotKnownSize_data();
    void testSetRawHeadersStart_data();
    void testSetRawHeadersWithRangeStart_data();
    void testSetRawHeadersResume_data();
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

    // process related tests
    void testProcessExecutedNoParams();
    void testProcessExecutedWithParams();
    void testProcessExecutedWithParamsFile();
    void testProcessFinishedWithError();
    void testProcessError_data();
    void testProcessError();
    void testProcessFinishedCrash();

    // test related to bug #1224678
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

    // test lock of files
    void testRedirectDoesNotUnlockPath();
    void testCancelUnlocksPath();
    void testFinishUnlocksPath();
    void testProcessFinishUnlocksPath();
    void testErrorUnlocksPath();
    void testLockCustomLocalPath();

    // tests related to setting the dir
    void testSetLocalDirectory();
    void testSetLocalDirectoryNotAbsolute();
    void testSetLocalDirectoryNotPresent();
    void testSetLocalDirectoryNotDir();
    void testSetLocalDirectoryStarted();

    // tests related to the deflate option
    void testDeflateConstructorError();
    void testDeflateConstructorNoError();
    void testDeflateOnRequest();

 private:
    QString _id;
    QString _appId;
    bool _isConfined;
    QString _rootPath;
    QVariantMap _metadata;
    QMap<QString, QString> _headers;
    QString _path;
    QUrl _url;
    QString _algo;
    MockSystemNetworkInfo* _networkInfo;
    MockRequestFactory* _reqFactory;
    MockProcessFactory* _processFactory;
    MockFileManager* _fileManager;
    MockCryptographicHashFactory* _cryptoFactory;
};

Q_DECLARE_METATYPE(QNetworkInfo::NetworkMode)

#endif  // TEST_APP_DOWNLOAD_H
