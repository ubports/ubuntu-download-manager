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
#ifndef TEST_DOWNLOADS_DB_H
#define TEST_DOWNLOADS_DB_H

#include <QDir>
#include <QObject>
#include <QSqlDatabase>
#include <downloads/downloads_db.h>
#include "base_testcase.h"
#include "test_runner.h"

using namespace Ubuntu::DownloadManager;

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

 private:
    DownloadsDb* _db;
};

DECLARE_TEST(TestDownloadsDb)

#endif  // TEST_DOWNLOADS_DB_H
