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

#include "download_adaptor_factory.h"
#include "download_manager_factory.h"
#include "manager.h"
#include "daemon.h"

namespace Ubuntu {

using namespace Transfers;

namespace DownloadManager {

namespace Daemon {

DownloadDaemon::DownloadDaemon(QObject *parent)
    : BaseDaemon(new DownloadManagerFactory(),
                 new DownloadAdaptorFactory(),
                 parent) {
}

DownloadDaemon::DownloadDaemon(ManagerFactory* managerFactory,
                               System::Application* app,
                               System::DBusConnection* conn,
                               System::Timer* timer,
                               QObject *parent)
    : BaseDaemon(managerFactory,
                 new DownloadAdaptorFactory(),
                 app, conn, timer, parent) {
}

void
DownloadDaemon::start() {
    BaseDaemon::start(DownloadManager::SERVICE_PATH);
}

void
DownloadDaemon::start(const QString& path) {
    BaseDaemon::start(path);
}

}  // Daemon

}  // DownloadManager

}  // Ubuntu
