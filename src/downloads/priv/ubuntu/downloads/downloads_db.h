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

#ifndef DOWNLOADER_LIB_DOWNLOADS_DATABASE_H
#define DOWNLOADER_LIB_DOWNLOADS_DATABASE_H

#include <QSqlDatabase>
#include <QObject>

#include <ubuntu/transfers/system/file_manager.h>
#include <ubuntu/download_manager/download_state_struct.h>

#include "file_download.h"

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

class DownloadsDb : public QObject {
    Q_OBJECT

 public:
    static DownloadsDb* instance();
    static void setStoppable(bool stoppable);

    // only used for testing purposes
    static void setInstance(DownloadsDb* instance);
    static void deleteInstance();

    QSqlDatabase db();
    QString filename();
    bool dbExists();  // return if the db is present and valid
    bool init();  // init or update the db

    virtual bool store(Download* down);
    virtual DownloadStateStruct getDownloadState(const QString &downloadId);
    virtual QList<Download*> getUncollectedDownloads(const QString &appId);

    bool storeSingleDownload(FileDownload* download);
    void connectToDownload(Download* download);
    void disconnectFromDownload(Download* download);

 public slots:
    void onDownloadChanged();

 protected:
    explicit DownloadsDb(QObject *parent = 0);

 private:
    QString headersToString(const QMap<QString, QString>& headers);
    void internalInit();
    QString metadataToString(const QVariantMap& metadata);
    QVariantMap stringToVariantMap(const QString &str);
    QMap<QString, QString> stringToStringMap(const QString &str);
    QString stateToString(Download::State state);
    Download::State stringToState(QString state);

 private:
    // used for the singleton
    static DownloadsDb* _instance;
    static QMutex _mutex;

    QString _dbName;
    FileManager* _fileManager;
    QSqlDatabase _db;
};

}  // Daemon

}  // DownloadManager

}  // Ubuntu

#endif
