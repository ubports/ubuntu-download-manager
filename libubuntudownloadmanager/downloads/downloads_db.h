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

#ifndef DOWNLOADER_LIB_DOWNLOADS_DATABASE_H
#define DOWNLOADER_LIB_DOWNLOADS_DATABASE_H

#include <QSqlDatabase>
#include <QObject>
#include "downloads/file_download.h"
#include "system/file_manager.h"

namespace Ubuntu {

namespace DownloadManager {

class DownloadsDb : public QObject {
    Q_OBJECT

 public:
    explicit DownloadsDb(QObject *parent = 0);

    QSqlDatabase db();
    QString filename();
    bool dbExists();  // return if the db is present and valid
    bool init();  // init or update the db
    bool storeSingleDownload(FileDownload* download);

 private:
    QString headersToString(const QMap<QString, QString>& headers);
    void internalInit();
    QString metadataToString(const QVariantMap& metadata);
    QString stateToString(Download::State state);
    Download::State stringToState(QString state);

 private:
    QString _dbName;
    FileManager* _fileManager;
    QSqlDatabase _db;
};

}  // DownloadManager

}  // Ubuntu

#endif  // DOWNLOADER_LIB_DOWNLOADS_DATABASE_H
