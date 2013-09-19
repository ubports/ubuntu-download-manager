/*
 * Copyright 2013 2013 Canonical Ltd.
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
#include <download_adaptor.h>
#include <download_queue.h>
#include "./test_runner.h"
#include "./fake_download.h"
#include "./fake_request_factory.h"
#include "./fake_process_factory.h"
#include "./fake_system_network_info.h"

class TestDownloadQueue : public QObject {
    Q_OBJECT

 public:
    explicit TestDownloadQueue(QObject *parent = 0);

 private slots:  // NOLINT(whitespace/indent)

    void init();
    void cleanup();

    void testAddDownload();
    void testStartDownloadWithNoCurrent();
    void testStartDownloadWithCurrent();
    void testStartDownloadWithNoCurrentCannotDownload();
    void testPauseDownloadNoOtherReady();
    void testPauseDownloadOtherReady();
    void testResumeDownloadNoOtherPresent();
    void testResumeDownloadOtherPresent();
    void testResumeDownloadNoOtherPresentCannotDownload();
    void testCancelDownloadNoOtherReady();
    void testCancelDownloadOtherReady();
    void testCancelDownloadOtherReadyCannotDownload();
    void testCancelDownloadNotStarted();
    void testDownloads();
    void testDownloadFinishedOtherReady();
    void testDownloadErrorWithOtherReady();

 private:
    bool _isConfined;
    QString _rootPath;
    FakeSystemNetworkInfo* _networkInfo;
    FakeRequestFactory* _reqFactory;
    FakeProcessFactory* _processFactory;
    FakeDownload* _first;
    FakeDownload* _second;
    DownloadQueue* _q;
};

DECLARE_TEST(TestDownloadQueue)

#endif  // TEST_DOWNLOAD_QUEUE_H
