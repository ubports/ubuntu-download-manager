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

#ifndef UBUNTU_DOWNLOADMANAGER_CLIENT_GROUP_DOWNLOAD_H
#define UBUNTU_DOWNLOADMANAGER_CLIENT_GROUP_DOWNLOAD_H

#include <QDBusObjectPath>
#include <QObject>
#include <ubuntu/download_manager/common.h>

namespace Ubuntu {

namespace DownloadManager {

class Error;
class ManagerPrivate;
class GroupDownloadPrivate;
class DOWNLOAD_MANAGER_EXPORT GroupDownload : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(GroupDownload)

    // allow the manager to create downloads
    friend class ManagerPrivate;
    friend class GroupManagerPendingCallWatcher;

 public:

    virtual ~GroupDownload();
    bool isError();
    Error* error();

 protected:
    GroupDownload(Error* err, QObject* parent = 0);
    GroupDownload(QDBusObjectPath path, QObject* parent = 0);

 private:
    // TODO(mandel): move this to pimpl, here to ensure no mem leaks atm
    Error* _error = NULL;
    // use pimpl pattern so that users do not have to be recompiled
    GroupDownloadPrivate* d_ptr;

};


}  // DownloadManager

}  // Ubuntu

#endif // GROUP_DOWNLOAD_H
