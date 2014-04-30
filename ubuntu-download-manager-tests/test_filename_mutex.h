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

#ifndef TEST_FILENAME_MUTEX_H
#define TEST_FILENAME_MUTEX_H

#include <QObject>
#include <ubuntu/download_manager/tests/base_testcase.h>

class TestFileNameMutex : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestFileNameMutex(QObject *parent = 0);

 private slots:  // NOLINT(whitespace/indent)

    void testExpectedNameValid_data();
    void testExpectedNameValid();
    void testExpectedNameLocked_data();
    void testExpectedNameLocked();
    void testExpectedNameInFileSystem_data();
    void testExpectedNameInFileSystem();
    void testUnlockPresent();
};

#endif // TEST_FILENAME_MUTEX_H
