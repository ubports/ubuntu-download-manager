/*
 * Copyright 2014 Canonical Ltd.
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

#ifndef TEST_DOWNLOADS_LIST_H
#define TEST_DOWNLOADS_LIST_H

#include <QObject>
#include <ubuntu/download_manager/download_list_impl.h>
#include <ubuntu/download_manager/tests/test_runner.h>
#include "local_tree_testcase.h"

using namespace Ubuntu::DownloadManager;

class TestDownloadsList : public LocalTreeTestCase {
    Q_OBJECT

 public:
    explicit TestDownloadsList(QObject *parent = 0);

 private slots:  // NOLINT(whitespace/indent)

    void testIsError();
    void testIsErrorEmptyList();
};

DECLARE_TEST(TestDownloadsList)

#endif // TEST_DOWNLOADS_LIST_H

