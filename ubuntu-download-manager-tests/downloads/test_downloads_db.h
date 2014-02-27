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
#ifndef TEST_DOWNLOADS_DB_H
#define TEST_DOWNLOADS_DB_H

#include <QDir>
#include <QObject>
#include <QSqlDatabase>
#include <ubuntu/downloads/downloads_db.h>
#include <ubuntu/transfers/tests/base_testcase.h>
#include <ubuntu/transfers/tests/test_runner.h>

using namespace Ubuntu::Transfers::Tests;
using namespace Ubuntu::Transfers::System;
using namespace Ubuntu::DownloadManager::Daemon;

// class used for testing some methods without the need of a real db
class TestingDb : public DownloadsDb {
    Q_OBJECT

 public:
    explicit TestingDb(QObject *parent = 0)
        : DownloadsDb(parent) {
    }

    bool store(Download* down) override {
        // just emit the signal and do nothing
        emit downloadStored(down);
        return true;
    }

 signals:
    void downloadStored(Download*);
};

class TestDownloadsDb : public BaseTestCase {
    Q_OBJECT
 public:
    explicit TestDownloadsDb(QObject *parent = 0);

 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;

    void testTableCreations_data();
    void testTableCreations();
    void testTableExists();
    void testStoreSingleDownload_data();
    void testStoreSingleDownload();
    void testStoreSingleDownloadPresent_data();
    void testStoreSingleDownloadPresent();
    void testConnectedToDownload();
    void testDisconnectedFromDownload();

 private:
    DownloadsDb* _db;
};

DECLARE_TEST(TestDownloadsDb)

#endif  // TEST_DOWNLOADS_DB_H
