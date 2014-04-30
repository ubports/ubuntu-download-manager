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

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <glog/logging.h>
#include <ubuntu/transfers/system/hash_algorithm.h>
#include <unistd.h>
#include "ubuntu/transfers/system/logger.h"
#include "downloads_db.h"

namespace {
    const QString SINGLE_DOWNLOAD_TABLE = "CREATE TABLE IF NOT EXISTS SingleDownload("\
        "uuid VARCHAR(40) PRIMARY KEY, "\
        "url TEXT NOT NULL, "\
        "dbus_path TEXT NOT NULL UNIQUE, "\
        "local_path TEXT, "\
        "hash TEXT, "\
        "hash_algo TEXT, "\
        "state VARCHAR(6) NOT NULL, "\
        "total_size TEXT, "\
        "throttle TEXT, "\
        "metadata TEXT, "\
        "headers TEXT)";

    const QString GROUP_DOWNLOAD_TABLE = "CREATE TABLE IF NOT EXISTS GroupDownload("\
        "uuid VARCHAR(40) PRIMARY KEY, "\
        "dbus_path TEXT NOT NULL UNIQUE, "\
        "state INTEGER NOT NULL, "\
        "total_size TEXT, "\
        "throttle TEXT, "\
        "metadata TEXT, "\
        "headers TEXT)";

    const QString GROUP_DOWNLOAD_RELATION = "CREATE TABLE IF NOT EXISTS GroupDownloadDownloads("\
        "group_id VARCHAR(36), "\
        "download_id VARCHAR(36), "\
        "FOREIGN KEY(group_id) REFERENCES GroupDownload(uuid), "\
        "FOREIGN KEY(download_id) REFERENCES SingleDownload(uuid))";

    const QString PRESENT_SINGLE_DOWNLOAD = "SELECT count(uuid) FROM SingleDownload "\
        "WHERE uuid=:uuid;";

    const QString INSERT_SINGLE_DOWNLOAD = "INSERT INTO SingleDownload("\
        "uuid, url, dbus_path, local_path, hash, hash_algo, state, total_size, "\
        "throttle, metadata, headers) VALUES (:uuid, :url, :dbus_path, "\
        ":local_path, :hash, :hash_algo, :state, :total_size, :throttle, "\
        ":metadata, :headers)";

    const QString UPDATE_SINGLE_DOWNLOAD = "UPDATE SingleDownload SET "\
        "url=:url, dbus_path=:dbus_path, local_path=:local_path, "\
        "hash=:hash, hash_algo=:hash_algo, state=:state, total_size=:total_size, "\
        "throttle=:throttle, metadata=:metadata, headers=:headers "\
        "WHERE uuid=:uuid";

    const QString IDLE_STRING = "idle";
    const QString START_STRING = "start";
    const QString PAUSE_STRING = "pause";
    const QString RESUME_STRING = "resume";
    const QString CANCEL_STRING = "cancel";
    const QString FINISH_STRING = "finish";
    const QString ERROR_STRING = "error";

}

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

DownloadsDb* DownloadsDb::_instance = nullptr;
QMutex DownloadsDb::_mutex;

DownloadsDb::DownloadsDb(QObject* parent)
    : QObject(parent) {
    _fileManager = FileManager::instance();
    internalInit();
}

QSqlDatabase
DownloadsDb::db() {
    return _db;
}

QString
DownloadsDb::filename() {
    return _dbName;
}

bool
DownloadsDb::dbExists() {
    return _fileManager->exists(_dbName);
}

void
DownloadsDb::internalInit() {
    QString path; 

    if (getuid() == 0) {
        path = "/var/cache";
    } else {
        QString dataPath = QStandardPaths::writableLocation(
            QStandardPaths::DataLocation);
        path = dataPath; 
    }

    path += QDir::separator() + QString("ubuntu-download-manager");
    bool wasCreated = QDir().mkpath(path);
    if (!wasCreated) {
        LOG(ERROR) << "Could not create the data path" << path;
    }
    _dbName = path + QDir::separator() + "downloads.db";
    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName(_dbName);
    LOG(INFO) << "Db file is " << _dbName;
}

bool
DownloadsDb::init() {
    TRACE;
    // create the required tables
    bool opened = _db.open();
    if (!opened) {
        LOG(ERROR) << _db.lastError().text();
        return false;
    }

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

QString
DownloadsDb::stateToString(Download::State state) {
    switch (state) {
        case Download::IDLE:
            return IDLE_STRING;
        case Download::START:
            return START_STRING;
        case Download::PAUSE:
            return PAUSE_STRING;
        case Download::RESUME:
            return RESUME_STRING;
        case Download::CANCEL:
            return CANCEL_STRING;
        case Download::FINISH:
            return FINISH_STRING;
        case Download::ERROR:
            return ERROR_STRING;
        default:
            return IDLE_STRING;
    }
}

Download::State
DownloadsDb::stringToState(QString state) {
    QString lowerState = state.toLower();
    if (lowerState == IDLE_STRING)
        return Download::IDLE;
    if (lowerState == START_STRING)
        return Download::START;
    if (lowerState == PAUSE_STRING)
        return Download::PAUSE;
    if (lowerState == RESUME_STRING)
        return Download::RESUME;
    if (lowerState == CANCEL_STRING)
        return Download::CANCEL;
    if (lowerState == FINISH_STRING)
        return Download::FINISH;
    if (lowerState == ERROR_STRING)
        return Download::ERROR;

    // default case
    return Download::IDLE;
}

QString
DownloadsDb::metadataToString(const QVariantMap& metadata) {
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(QVariant(metadata));
    return QString(jsonDoc.toJson());
}

QString
DownloadsDb::headersToString(const QMap<QString, QString>& headers) {
    QVariantMap headersVariant;
    foreach(const QString& key, headers.keys()) {
        headersVariant[key] = headers[key];
    }
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(
        QVariant(headersVariant));
    return QString(jsonDoc.toJson());
}

bool
DownloadsDb::store(Download* down) {
    auto fileDown = qobject_cast<FileDownload*>(down);
    if (fileDown != nullptr) {
        return storeSingleDownload(fileDown);
    }
    return false;
}

bool
DownloadsDb::storeSingleDownload(FileDownload* download) {
    // decide if we store it as a new download or update an existing one
    bool opened = _db.open();

    if (!opened) {
        LOG(ERROR) << _db.lastError().text();
        return false;
    }

    QSqlQuery query;
    query.prepare(PRESENT_SINGLE_DOWNLOAD);
    query.bindValue(":uuid", download->transferId());

    query.exec();
    int rows = 0;
    if (query.next())
        rows = query.value(0).toInt();

    if (rows > 0) {
        LOG(INFO) << "Update download";
        query.prepare(UPDATE_SINGLE_DOWNLOAD);
    } else {
        LOG(INFO) << "Insert download";
        query.prepare(INSERT_SINGLE_DOWNLOAD);
    }

    query.bindValue(":uuid", download->transferId());
    query.bindValue(":url", download->url().toString());
    query.bindValue(":dbus_path", download->path());
    query.bindValue(":local_path", download->filePath());
    query.bindValue(":hash", download->hash());
    query.bindValue(":hash_algo",
        HashAlgorithm::getHashAlgo(download->hashAlgorithm()));
    query.bindValue(":state", stateToString(download->state()));
    query.bindValue(":total_size",
        QString::number(download->totalSize()));
    query.bindValue(":throttle",
        QString::number(download->throttle()));
    query.bindValue(":metadata",
        metadataToString(download->metadata()));
    query.bindValue(":headers",
        headersToString(download->headers()));

    bool success = query.exec();
    if (!success)
        LOG(ERROR) << query.lastError().text();

    _db.close();

    return success;
}

void
DownloadsDb::connectToDownload(Download* download) {
    CHECK(connect(download, &Download::stateChanged,
        this, &DownloadsDb::onDownloadChanged))
            << "Could not connect to signal";
    CHECK(connect(download, &Download::throttleChanged,
        this, &DownloadsDb::onDownloadChanged))
            << "Could not connect to signal";
}

void
DownloadsDb::disconnectFromDownload(Download* download) {
    disconnect(download, &Download::stateChanged,
        this, &DownloadsDb::onDownloadChanged);
    disconnect(download, &Download::throttleChanged,
        this, &DownloadsDb::onDownloadChanged);
}

void
DownloadsDb::onDownloadChanged() {
    auto down = qobject_cast<Download*>(sender());
    if (down != nullptr) {
        store(down);
        auto state = down->state();
        if (state == Download::FINISH
                || state == Download::CANCEL
                || state == Download::ERROR) {
            LOG(INFO) << "Disconnecting from" << down->transferId();
            disconnectFromDownload(down);
        }
    }
}

DownloadsDb*
DownloadsDb::instance() {
    if(_instance == nullptr) {
        _mutex.lock();
        if(_instance == nullptr)
            _instance = new DownloadsDb();
            _instance->init();
        _mutex.unlock();
    }
    return _instance;
}

void
DownloadsDb::setInstance(DownloadsDb* instance) {
    _instance = instance;
}

void
DownloadsDb::deleteInstance() {
    if(_instance != nullptr) {
        _mutex.lock();
        if(_instance != nullptr) {
            delete _instance;
            _instance = nullptr;
        }
        _mutex.unlock();
    }
}


}  // Daemon

}  // DownloadManager

}  // Ubuntu
