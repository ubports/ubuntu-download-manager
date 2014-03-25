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

#ifndef TEST_FILE_DOWNLOAD_SM_H
#define TEST_FILE_DOWNLOAD_SM_H

#include <QObject>
#include <QState>
#include <QStateMachine>
#include <QSignalTransition>
#include <ubuntu/downloads/state_machines/download_sm.h>
#include "base_testcase.h"
#include "sm_file_download.h"

using namespace Ubuntu::DownloadManager::Daemon::StateMachines;

class TestFileDownloadSM : public BaseTestCase {
    Q_OBJECT
 public:
    explicit TestFileDownloadSM(QObject *parent = 0)
        : BaseTestCase("TestFileDownloadSM", parent) {}

 private:
    void moveToInit();
    void moveToDownloading();
    void moveToDownloadingNotConnected();
    void moveToPaused();
    void moveToPausedNotConnected();
    void moveToDownloaded();
    void moveToHashing();
    void moveToPostProcessing();

 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;

    // idle transitions
    void testIdleError();
    void testIdleSslErrors();
    void testIdleHeadRequestCompleted();

    // init transitions
    void testInitError();
    void testInitSslErrors();
    void testInitDownloadingStarted();

    // downloading
    void testDownloadingError();
    void testDownloadingSslErrors();
    void testDownloadingCanceled();
    void testDownloadingPaused();
    void testDownloadingConnectionLost();
    void testDownloadingDownloaded();

    // downloading not connected
    void testDownloadingNotConnectedConnectionEnabled();
    void testDownloadingNotConnectedPaused();
    void testDownloadingNotConnectedCanceled();

    // pause
    void testPauseDownloadingStarted();
    void testPauseConnectionLost();
    void testPauseCanceled();

    // pause not connected
    void testPausedNotConnectedConnectionEnabled();
    void testPausedNotConnectedDownloadingStarted();
    void testPausedNotConnectedCanceled();

    // downloaded
    void testDownloadedHashingStarted();
    void testDownloadedPostProcessingStarted();
    void testDownloadedFinished();
    void testDownloadedCanceled();

    // hashing
    void testHashingError();
    void testHashingFinished();
    void testHashingPostProcessing();

    // post processing
    void testPostProcessingError();
    void testPostProcessingFinished();

 private:
    MockSMFileDownload* _down;
    DownloadSM* _stateMachine;
};

#endif // TEST_FILE_DOWNLOAD_SM_H
