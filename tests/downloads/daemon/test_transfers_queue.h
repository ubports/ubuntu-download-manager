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

#ifndef TEST_DOWNLOAD_QUEUE_H
#define TEST_DOWNLOAD_QUEUE_H

#include <QObject>
#include <ubuntu/transfers/queue.h>
#include <process_factory.h>
#include <request_factory.h>
#include <network_session.h>

#include "base_testcase.h"
#include "transfer.h"

using namespace Ubuntu::Transfers;
using namespace Ubuntu::Transfers::Tests;
using namespace Ubuntu::Transfers::System;

class TestTransferQueue : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestTransferQueue(QObject *parent = 0)
        : BaseTestCase("TestTransferQueue", parent) {}

 private:
    void verifyMocks();

 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;

    void testAddTransfer();
    void testStartTransferWithNoCurrent();
    void testStartTransferWithCurrent();
    void testStartTransferWithNoCurrentCannotTransfer();
    void testPauseTransferNoOtherReady();
    void testPauseTransferOtherReady();
    void testResumeTransferNoOtherPresent();
    void testResumeTransferOtherPresent();
    void testResumeTransferNoOtherPresentCannotTransfer();
    void testCancelTransferNoOtherReady();
    void testCancelTransferOtherReady();
    void testCancelTransferOtherReadyCannotTransfer();
    void testCancelTransferNotStarted();
    void testTransfers();
    void testTransferFinishedOtherReady();
    void testTransferErrorWithOtherReady();

    // unmanaged downloads tests
    void testNewUnmanagedIncreasesNumber();
    void testErrorUnmanagedDecreasesNumber();
    void testFinishUnmanagedDecreasesNumber();
    void testCancelUnmanagedDecreasesNumber();

 private:
    bool _isConfined;
    QString _rootPath;
    MockNetworkSession* _networkInfo;
    MockTransfer* _first;
    MockTransfer* _second;
    Queue* _q;
};

#endif  // TEST_DOWNLOAD_QUEUE_H
