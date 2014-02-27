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

#ifndef TEST_DOWNLOAD_WATCH_H
#define TEST_DOWNLOAD_WATCH_H

#include <functional>
#include <ubuntu/download_manager/download.h>
#include <ubuntu/download_manager/error.h>
#include <ubuntu/download_manager/manager.h>
#include <QObject>
#include <ubuntu/transfers/tests/test_runner.h>
#include "local_tree_testcase.h"

using namespace Ubuntu::DownloadManager;

class TestDownloadManagerWatch : public LocalTreeTestCase {
    Q_OBJECT

 public:
    explicit TestDownloadManagerWatch(QObject *parent = 0);

 private:
    void onSuccessCb(Download* down);
    void onErrorCb(Download* err);
    void onDownloadError(Download* down);

 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;
    void testCallbackIsExecuted();
    void testErrCallbackIsExecuted();

 private:
    bool _calledSuccess = false;
    bool _calledError = false;
    Manager* _manager = nullptr;
};

DECLARE_TEST(TestDownloadManagerWatch)

#endif // TEST_DOWNLOAD_WATCH_H
