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

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "./test_downloads_db.h"

#define TABLE_EXISTS "SELECT count(name) FROM sqlite_master "\
    "WHERE type='table' AND name=:table_name;"

TestDownloadsDb::TestDownloadsDb(QObject *parent)
    : QObject(parent) {
}

bool
TestDownloadsDb::removeDir(const QString& dirName) {
    bool result = true;
    QDir dir(dirName);

    QFlags<QDir::Filter> filter =  QDir::NoDotAndDotDot | QDir::System
        | QDir::Hidden  | QDir::AllDirs | QDir::Files;
    if (dir.exists(dirName)) {
        foreach(QFileInfo info, dir.entryInfoList(filter, QDir::DirsFirst)) {
            qDebug() << "Removing" << info.absoluteFilePath();
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            } else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}

void
TestDownloadsDb::init() {
    _testDir = QDir("./tests");
    _testDir.makeAbsolute();

    if (!_testDir.exists()) {
        _testDir.mkpath(_testDir.absolutePath());
    }

    setenv("XDG_DATA_HOME",
        _testDir.absolutePath().toStdString().c_str(), 1);

    _db = new DownloadsDb();
}

void
TestDownloadsDb::cleanup() {
    // try to remove the test dir
    if (_db != NULL)
        delete _db;
    removeDir(_testDir.absolutePath());
    unsetenv("XDG_DATA_HOME");
}

void
TestDownloadsDb::testTableCreations_data() {
    QTest::addColumn<QString>("tableName");

    QTest::newRow("SingleDownload table present") << "SingleDownload";
    QTest::newRow("GroupDownload table present") << "GroupDownload";
    QTest::newRow("GroupDownload realtion table present")
        << "GroupDownloadDownloads";
}

void
TestDownloadsDb::testTableCreations() {
    QFETCH(QString, tableName);
    _db->init();
    QSqlDatabase db = _db->db();
    db.open();
    // open the db with the filename and assert that all tables are
    // present
    QSqlQuery query(db);
    query.prepare(TABLE_EXISTS);
    query.bindValue(":table_name", tableName);

    query.exec();
    int rows = 0;
    if (query.next())
        rows = query.value(0).toInt();
    db.close();
    QCOMPARE(1, rows);
}

void
TestDownloadsDb::testTableExists() {
    _db->init();
    QVERIFY(QFile::exists(_db->filename()));
}
