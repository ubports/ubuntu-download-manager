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

#ifndef DOWNLOADER_LIB_GROUP_DOWNLOAD_H
#define DOWNLOADER_LIB_GROUP_DOWNLOAD_H

#include <QCryptographicHash>
#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <ubuntu/download_manager/group_download_struct.h>
#include "downloads/download.h"
#include "downloads/factory.h"
#include "downloads/file_download.h"
#include "system/file_manager.h"

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

class GroupDownload : public Download {
    Q_OBJECT

 public:
    GroupDownload(const QString& id,
                  const QString& path,
                  bool isConfined,
                  const QString& rootPath,
                  QList<GroupDownloadStruct> downloads,
                  const QString& algo,
                  bool isGSMDownloadAllowed,
                  const QVariantMap& metadata,
                  const QMap<QString, QString>& headers,
                  Factory* downFactory,
                  QObject* parent = 0);
    virtual ~GroupDownload();

    virtual void cancelDownload() override;
    virtual void pauseDownload() override;
    virtual void resumeDownload() override;
    virtual void startDownload() override;

 public slots:  // NOLINT(whitespace/indent)
    virtual qulonglong progress() override;
    virtual qulonglong progress(qulonglong &started, qulonglong &paused,
        qulonglong &finished);
    virtual qulonglong totalSize() override;

 signals:
    void finished(const QStringList &path);

 private:
    void cancelAllDownloads();
    void connectToDownloadSignals(FileDownload* singleDownload);
    void init(QList<GroupDownloadStruct> downloads,
              const QString& algo,
              bool isGSMDownloadAllowed);
    void onError(const QString& error);
    void onProgress(qulonglong received, qulonglong total);
    void onFinished(const QString& file);

 private:
    QList<FileDownload*> _downloads;
    QStringList _finishedDownloads;
    QMap<QUrl, QPair<qulonglong, qulonglong> > _downloadsProgress;
    Factory* _downFactory = NULL;
    FileManager* _fileManager = NULL;
};

}  // Daemon

}  // DownloadManager

}  // Ubuntu

#endif  // DOWNLOADER_LIB_GROUP_DOWNLOAD_H
