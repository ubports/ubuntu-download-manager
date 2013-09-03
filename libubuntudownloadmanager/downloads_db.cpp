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

#include "./downloads_db.h"

#define SINGLE_DOWNLOAD_TABLE "CREATE TABLE SingleDownload("\
    "id INTERGER PRIMARY KEY,"\
    "uuid TEXT NOT NULL UNIQUE,"\
    "url TEXT NOT NULL,"\
    "dbus_path TEXT NOT NULL UNIQUE,"\
    "local_path TEXT,"\
    "hash TEXT,"\
    "hash_algo TEXT,"\
    "state INT NOT NULL,"\
    "total_size TEXT,"\
    "throttle TEXT,"\
    "metadata TEXT,"\
    "headers TEXT);"

class DownloadsDBPrivate {
    Q_DECLARE_PUBLIC(DownloadsDB)

 public:
    explicit DownloadsDBPrivate(DownloasdDB* parent) {
    }


    bool dbExists() {
    }

    bool init() {
    }
};

/*
 * PUBLIC IMPLEMENTATION
 */

DownloasdDB::DownloasdDB(QObject *parent) {
}

bool
DownloasdDB::dbExists() {
    Q_D();
    return d->dbExists();
}

bool
DownloasdDB::init() {
    Q_D();
    return d->init();
}
