/*
 * Copyright 2013 Canonical Ltd.
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
#include "./download.h"
#include "./download_factory.h"
#include "./file_manager.h"
#include "./group_download_struct.h"

class GroupDownloadPrivate;
class GroupDownload : public Download {
    Q_OBJECT
    Q_DECLARE_PRIVATE(GroupDownload)

 public:
    GroupDownload(const QUuid& id,
                  const QString& path,
                  bool isConfined,
                  const QString& rootPath,
                  QList<GroupDownloadStruct> downloads,
                  const QString& algo,
                  bool isGSMDownloadAllowed,
                  const QVariantMap& metadata,
                  const QMap<QString, QString>& headers,
                  QSharedPointer<SystemNetworkInfo> networkInfo,
                  QSharedPointer<DownloadFactory> downFactory,
                  QObject* parent = 0);

    GroupDownload(const QUuid& id,
                  const QString& path,
                  bool isConfined,
                  const QString& rootPath,
                  QList<GroupDownloadStruct> downloads,
                  const QString& algo,
                  bool isGSMDownloadAllowed,
                  const QVariantMap& metadata,
                  const QMap<QString, QString>& headers,
                  QSharedPointer<SystemNetworkInfo> networkInfo,
                  QSharedPointer<DownloadFactory> downFactory,
                  QSharedPointer<FileManager> fileManager,
                  QObject* parent = 0);

    virtual void cancelDownload() override;
    virtual void pauseDownload() override;
    virtual void resumeDownload() override;
    virtual void startDownload() override;

    virtual bool isValid() override;
    virtual QString lastError() override;

 public slots:  // NOLINT(whitespace/indent)
    virtual qulonglong progress() override;
    virtual qulonglong progress(qulonglong &started, qulonglong &paused,
        qulonglong &finished);
    virtual qulonglong totalSize() override;

 signals:
    void finished(const QStringList &path);

 private:
    // private slots used to keep track of the diff downloads

    Q_PRIVATE_SLOT(d_func(), void onError(const QString& error))
    Q_PRIVATE_SLOT(d_func(), void onProgress(qulonglong received,
        qulonglong total))
    Q_PRIVATE_SLOT(d_func(), void onFinished(const QString& file))

 private:
    // use pimpl so that we can mantains ABI compatibility
    GroupDownloadPrivate* d_ptr;
};

#endif  // DOWNLOADER_LIB_GROUP_DOWNLOAD_H
