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

#ifndef UBUNTU_DOWNLOADMANAGER_CLIENT_MANAGER_H
#define UBUNTU_DOWNLOADMANAGER_CLIENT_MANAGER_H

#include <functional>
#include <QObject>
#include <ubuntu/download_manager/metatypes.h>
#include <ubuntu/download_manager/common.h>
#include <ubuntu/download_manager/download_struct.h>
#include <ubuntu/download_manager/group_download_struct.h>
#include "download.h"
#include "download_list.h"
#include "error.h"


namespace Ubuntu {

namespace DownloadManager {

class GroupDownload;

typedef std::function<void(Download*)> DownloadCb;
typedef std::function<void(DownloadList*)> DownloadsListCb;
typedef std::function<void(const QString&, const QString&, DownloadList*)> MetadataDownloadsListCb;
typedef std::function<void(GroupDownload*)> GroupCb;

class DOWNLOAD_MANAGER_EXPORT Manager : public QObject {
    Q_OBJECT

 public:
    explicit Manager(QObject* parent = 0)
        : QObject(parent) {}

    virtual void createDownload(DownloadStruct downStruct) = 0;
    virtual void createDownload(DownloadStruct downStruct,
                                DownloadCb cb,
                                DownloadCb errCb) = 0;
    virtual void createDownload(StructList downs,
                                const QString &algorithm,
                                bool allowed3G,
                                const QVariantMap &metadata,
                                StringMap headers) = 0;
    virtual void createDownload(StructList downs,
                                const QString &algorithm,
                                bool allowed3G,
                                const QVariantMap &metadata,
                                StringMap headers,
                                GroupCb cb,
                                GroupCb errCb) = 0;

    virtual void getAllDownloads() = 0;
    virtual void getAllDownloads(DownloadsListCb cb,
                                 DownloadsListCb errCb) = 0;
    virtual void getAllDownloadsWithMetadata(const QString &name,
                                             const QString &value) = 0;
    virtual void getAllDownloadsWithMetadata(const QString &name,
                                             const QString &value,
                                             MetadataDownloadsListCb cb,
                                             MetadataDownloadsListCb errCb) = 0;

    virtual bool isError() const = 0;
    virtual Error* lastError() const = 0;
    virtual void allowMobileDataDownload(bool allowed) = 0;
    virtual bool isMobileDataDownload() = 0;
    virtual qulonglong defaultThrottle() = 0;
    virtual void setDefaultThrottle(qulonglong speed) = 0;
    virtual void exit() = 0;

    static Manager* createSessionManager(const QString& path = "", QObject* parent=0);
    static Manager* createSystemManager(const QString& path = "", QObject* parent=0);

 signals:
    void downloadCreated(Download* down);
    void downloadsFound(DownloadList* downloads);
    void downloadsWithMetadataFound(const QString& name,
                                    const QString& value,
                                    DownloadList* downloads);
    void groupCreated(GroupDownload* down);

};

}  // DownloadManager

}  // Ubuntu

#endif  // UBUNTU_DOWNLOADMANAGER_CLIENT_MANAGER_H
