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

#ifndef UBUNTU_DOWNLOADMANAGER_CLIENT_MANAGER_H
#define UBUNTU_DOWNLOADMANAGER_CLIENT_MANAGER_H

#include <functional>
#include <QObject>
#include <ubuntu/download_manager/download_struct.h>
#include <ubuntu/download_manager/group_download_struct.h>
#include "ubuntu-download-manager-client_global.h"

namespace Ubuntu {

namespace DownloadManager {

class Download;
class GroupDownload;

typedef std::function<void(Download*)> DownloadCreationCb;
typedef std::function<void(GroupDownload*)> GroupCreationCb;

class ManagerPrivate;
class UBUNTUDOWNLOADMANAGERCLIENTSHARED_EXPORT Manager : public QObject {
    Q_DECLARE_PRIVATE(Manager)
    Q_OBJECT

 public:
    explicit Manager(QObject* parent);

    virtual Download* createDownload(DownloadStruct downStruct);
    virtual void createDownload(DownloadStruct downStrcut, DownloadCreationCb cb);
    virtual GroupDownload* createDownload(GroupDownloadStruct  groupStruct);
    virtual void createDownload(GroupDownloadStruct  groupStruct, GroupCreationCb cb);

 signals:
    void downloadCreated(Download* down);
    void groupCreated(GroupDownload* down);

 private:
    // use pimpl pattern so that users do not have to be recompiled
    ManagerPrivate* d_ptr;
};

}  // DownloadManager

}  // Ubuntu

#endif  // UBUNTU_DOWNLOADMANAGER_CLIENT_MANAGER_H
