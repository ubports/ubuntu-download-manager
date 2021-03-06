/*
 * Copyright 2014 Canonical Ltd.
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

#include "upload_adaptor_factory.h"
#include "upload_manager_factory.h"
#include "manager.h"
#include "daemon.h"

namespace Ubuntu {

using namespace Transfers;

namespace UploadManager {

namespace Daemon {

UploadDaemon::UploadDaemon(QObject *parent)
    : BaseDaemon(new UploadManagerFactory(),
                 new UploadAdaptorFactory(),
                 parent) {
}

UploadDaemon::UploadDaemon(ManagerFactory* managerFactory,
                           System::Application* app,
                           System::DBusConnection* conn,
                           System::Timer* timer,
                           QObject *parent)
    : BaseDaemon(managerFactory,
                 new UploadAdaptorFactory(),
                 app, conn, timer, parent) {
}

void
UploadDaemon::start() {
    BaseDaemon::start(UploadManager::SERVICE_PATH);
}

void
UploadDaemon::start(const QString& path) {
    BaseDaemon::start(path);
}

}  // Daemon

}  // UploadManager

}  // Ubuntu

