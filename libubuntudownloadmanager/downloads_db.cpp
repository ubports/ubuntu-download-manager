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
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "./xdg_basedir.h"
#include "./downloads_db.h"

#define SINGLE_DOWNLOAD_TABLE "CREATE TABLE SingleDownload("\
    "id INTERGER PRIMARY KEY, "\
    "uuid TEXT NOT NULL UNIQUE, "\
    "url TEXT NOT NULL, "\
    "dbus_path TEXT NOT NULL UNIQUE, "\
    "local_path TEXT, "\
    "hash TEXT, "\
    "hash_algo TEXT, "\
    "state INT NOT NULL, "\
    "total_size TEXT, "\
    "throttle TEXT, "\
    "metadata TEXT, "\
    "headers TEXT)"

#define GROUP_DOWNLOAD_TABLE "CREATE TABLE GroupDownload("\
    "id INTERGER PRIMARY KEY, "\
    "uuid TEXT NOT NULL UNIQUE, "\
    "dbus_path TEXT NOT NULL UNIQUE, "\
    "state INT NOT NULL, "\
    "total_size TEXT, "\
    "throttle TEXT, "\
    "metadata TEXT, "\
    "headers TEXT)"

#define GROUP_DOWNLOAD_RELATION "CREATE TABLE GroupDownloadDownloads("\
    "group_id INTEGER, "\
    "download_id INTEGER, "\
    "FOREIGN KEY(group_id) REFERENCES GroupDownload(id), "\
    "FOREIGN KEY(download_id) REFERENCES SingleDownload(id))"

class DownloadsDbPrivate {
    Q_DECLARE_PUBLIC(DownloadsDb)

 public:
    explicit DownloadsDbPrivate(DownloadsDb* parent)
        : q_ptr(parent) {
        _fileManager = new FileManager();
        internalInit();
    }

    DownloadsDbPrivate(FileManager* fileManager, DownloadsDb* parent)
        : _fileManager(fileManager),
          q_ptr(parent) {
        internalInit();
    }

    QSqlDatabase db() {
        return _db;
    }

    QString filename() {
        return _dbName;
    }

    bool dbExists() {
        return _fileManager->exists(_dbName);
    }

    void internalInit() {
        QStringList pathComponents;
        pathComponents << "download_manager";
        _dbName = XDGBasedir::saveDataPath(pathComponents) + "/downloads.db";
        _db = QSqlDatabase::addDatabase("QSQLITE");
        _db.setDatabaseName(_dbName);
        qDebug() << "Db file is" << _dbName;
    }

    bool init() {
        qDebug() << __PRETTY_FUNCTION__;
        // create the required tables
        qDebug() << "open the db" << _db.open();

        _db.transaction();

        // create the required tables and indexes
        bool success = true;
        QSqlQuery query;
        success &= query.exec(SINGLE_DOWNLOAD_TABLE);
        success &= query.exec(GROUP_DOWNLOAD_TABLE);
        success &= query.exec(GROUP_DOWNLOAD_RELATION);

        if (success)
            _db.commit();
        else
            _db.rollback();
        _db.close();
        return success;
    }

 private:
    QString _dbName;
    FileManager* _fileManager;
    QSqlDatabase _db;
    DownloadsDb* q_ptr;
};

/*
 * PUBLIC IMPLEMENTATION
 */

DownloadsDb::DownloadsDb(QObject *parent)
    : QObject(parent),
      d_ptr(new DownloadsDbPrivate(this)) {
}

DownloadsDb::DownloadsDb(FileManager* fileManager, QObject *parent)
    : QObject(parent),
      d_ptr(new DownloadsDbPrivate(fileManager, this)) {
}

QSqlDatabase
DownloadsDb::db() {
    Q_D(DownloadsDb);
    return d->db();
}

QString
DownloadsDb::filename() {
    Q_D(DownloadsDb);
    return d->filename();
}

bool
DownloadsDb::dbExists() {
    Q_D(DownloadsDb);
    return d->dbExists();
}

bool
DownloadsDb::init() {
    Q_D(DownloadsDb);
    return d->init();
}
