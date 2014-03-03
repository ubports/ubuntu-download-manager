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

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QScopedPointer>
#include <ubuntu/transfers/system/filename_mutex.h>
#include "test_filename_mutex.h"

using namespace Ubuntu::Transfers::System;

TestFileNameMutex::TestFileNameMutex(QObject *parent)
    : BaseTestCase("TestFileNameMutex", parent) {
}

void
TestFileNameMutex::testExpectedNameValid_data() {
    QTest::addColumn<QString>("path");
    auto dir = testDirectory();

    QTest::newRow("First path") << dir + QDir::separator() + "first.zip";
    QTest::newRow("Second path") << dir + QDir::separator() + "second.zip";
    QTest::newRow("Last path") << dir + QDir::separator() + "last.zip";
}

void
TestFileNameMutex::testExpectedNameValid() {
    QFETCH(QString, path);

    QScopedPointer<FileNameMutex> mutex(new FileNameMutex());
    auto locked = mutex->lockFileName(path);
    QCOMPARE(path, locked);
    QVERIFY(mutex->isLocked(path));
}

void
TestFileNameMutex::testExpectedNameLocked_data() {
    QTest::addColumn<QString>("path");
    auto dir = testDirectory();

    QTest::newRow("First path") << dir + QDir::separator() + "first.zip";
    QTest::newRow("Second path") << dir + QDir::separator() + "second.zip";
    QTest::newRow("Last path") << dir + QDir::separator() + "last.zip";
}

void
TestFileNameMutex::testExpectedNameLocked() {
    QFETCH(QString, path);

    QScopedPointer<FileNameMutex> mutex(new FileNameMutex());
    mutex->lockFileName(path);  // ensure that we already locked the path
    auto locked = mutex->lockFileName(path);
    QVERIFY(path != locked);
    QVERIFY(mutex->isLocked(locked));
}

void
TestFileNameMutex::testExpectedNameInFileSystem_data() {
    QTest::addColumn<QString>("path");
    auto dir = testDirectory();

    QTest::newRow("First path") << dir + QDir::separator() + "first.zip";
    QTest::newRow("Second path") << dir + QDir::separator() + "second_zip";
    QTest::newRow("Last path") << dir + QDir::separator() + "last_zip";
}

void
TestFileNameMutex::testExpectedNameInFileSystem() {
    QFETCH(QString, path);
    testDirectory();  // build temp dir
    QScopedPointer<FileNameMutex> mutex(new FileNameMutex());

    QFile file(path);
    file.open(QIODevice::ReadWrite | QFile::Append);
    file.write(QByteArray(400, 'f'));
    file.flush();
    file.close();

    auto locked = mutex->lockFileName(path);
    QVERIFY(path != locked);
    QVERIFY(!mutex->isLocked(path));
}

void
TestFileNameMutex::testUnlockPresent() {
    auto path = testDirectory() + QDir::separator() + "test.txt";
    QScopedPointer<FileNameMutex> mutex(new FileNameMutex());
    auto locked = mutex->lockFileName(path);
    QVERIFY(mutex->isLocked(locked));
    mutex->unlockFileName(path);
    QVERIFY(!mutex->isLocked(locked));
}
