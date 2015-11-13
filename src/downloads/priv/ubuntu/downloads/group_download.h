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
#include <ubuntu/download_manager/group_download_struct.h>
#include "ubuntu/transfers/system/file_manager.h"
#include "download.h"
#include "factory.h"
#include "file_download.h"

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

class Factory;
class GroupDownload : public Download {
    Q_OBJECT

 public:
    GroupDownload(const QString& id,
                  const QString& appId,
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

    virtual void cancelTransfer() override;
    virtual void pauseTransfer() override;
    virtual void resumeTransfer() override;
    virtual void startTransfer() override;

 public slots:  // NOLINT(whitespace/indent)
    virtual qulonglong progress() override;
    virtual qulonglong progress(qulonglong &started, qulonglong &paused,
        qulonglong &finished);
    virtual qulonglong totalSize() override;
    virtual QString filePath() override;

 signals:
    void finished(const QStringList &path);
    void authError(const QString &url, AuthErrorStruct error);
    void httpError(const QString &url, HttpErrorStruct error);
    void networkError(const QString &url, NetworkErrorStruct error);
    void processError(const QString &url, ProcessErrorStruct error);

 private:
    void cancelAllDownloads();
    void connectToDownloadSignals(FileDownload* singleDownload);
    void init(QList<GroupDownloadStruct> downloads,
              const QString& algo,
              bool isGSMDownloadAllowed);
    QString getUrlFromSender(QObject* sender);
    void onProgress(qulonglong received, qulonglong total);
    void onFinished(const QString& file);
    void onError(const QString& error);
    void onCanceled();
    void onAuthError(AuthErrorStruct err);
    void onHttpError(HttpErrorStruct err);
    void onNetworkError(NetworkErrorStruct err);
    void onProcessError(ProcessErrorStruct err);

 protected:
    QList<FileDownload*> _downloads;

 private:
    QStringList _finishedDownloads;
    QMap<QUrl, QPair<qulonglong, qulonglong> > _downloadsProgress;
    Factory* _downFactory = nullptr;
    FileManager* _fileManager = nullptr;
};

}  // Daemon

}  // DownloadManager

}  // Ubuntu

#endif  // DOWNLOADER_LIB_GROUP_DOWNLOAD_H
