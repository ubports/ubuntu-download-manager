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

#ifndef TEST_GROUP_WATCH_H
#define TEST_GROUP_WATCH_H

#include <functional>
#include <ubuntu/download_manager/group_download.h>
#include <ubuntu/download_manager/error.h>
#include <ubuntu/download_manager/manager.h>
#include <QObject>
#include <ubuntu/transfers/tests/test_runner.h>
#include "local_tree_testcase.h"

using namespace Ubuntu::DownloadManager;

class TestGroupManagerWatch : public LocalTreeTestCase {
    Q_OBJECT

 public:
    explicit TestGroupManagerWatch(QObject* parent = 0);

 private:
    void onSuccessCb(GroupDownload* down);
    void onErrorCb(GroupDownload* err);

 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;
    void testCallbackIsExecuted();
    void testErrCallbackIsExecuted();

 private:
    bool _calledSuccess = false;
    bool _calledError = false;
    QString _algo;
    QVariantMap _metadata;
    QMap<QString, QString> _headers;
    Manager* _manager = nullptr;
};

DECLARE_TEST(TestGroupManagerWatch)

#endif // TEST_GROUP_WATCH_H
