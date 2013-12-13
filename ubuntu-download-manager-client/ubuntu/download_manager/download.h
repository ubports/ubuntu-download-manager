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

#ifndef UBUNTU_DOWNLOADMANAGER_CLIENT_DOWNLOAD_H
#define UBUNTU_DOWNLOADMANAGER_CLIENT_DOWNLOAD_H

#include <QDBusObjectPath>
#include <QObject>
#include "ubuntu-download-manager-client_global.h"

namespace Ubuntu {

namespace DownloadManager {

class Error;
class ManagerPrivate;
class DownloadPrivate;
class UBUNTUDOWNLOADMANAGERCLIENTSHARED_EXPORT Download : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(Download)

    // allow the manager to create downloads
    friend class ManagerPrivate;
    friend class DownloadManagerPendingCallWatcher;

    bool isError();
    Error* error();

 protected:
    Download(Error* err, QObject* parent = 0);
    Download(QDBusObjectPath path, QObject* parent = 0);

 private:
    // use pimpl pattern so that users do not have to be recompiled
    DownloadPrivate* d_ptr;

};

}  // Ubuntu

}  // DownloadManager

#endif  // UBUNTU_DOWNLOADMANAGER_CLIENT_DOWNLOAD_H
