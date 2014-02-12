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

#include <QDebug>
#include "manager_impl.h"
#include "manager.h"

namespace {
    const QString DOWNLOAD_SERVICE = "com.canonical.applications.Downloader";
}

namespace Ubuntu {

namespace DownloadManager {

Manager*
Manager::createSessionManager(const QString& path, QObject* parent) {
    if (path.isEmpty()) {
        return new ManagerImpl(QDBusConnection::sessionBus(), DOWNLOAD_SERVICE, parent);
    } else {
        return new ManagerImpl(QDBusConnection::sessionBus(), path, parent);
    }
}

Manager*
Manager::createSystemManager(const QString& path, QObject* parent) {
    if (path.isEmpty()) {
        return new ManagerImpl(QDBusConnection::systemBus(), DOWNLOAD_SERVICE, parent);
    } else {
        return new ManagerImpl(QDBusConnection::systemBus(), path, parent);
    }
}


}  // DownloadManager

}  // Ubuntu
