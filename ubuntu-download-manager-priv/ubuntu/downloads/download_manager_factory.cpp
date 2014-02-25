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

#include "manager.h"
#include "download_manager_factory.h"

namespace Ubuntu {

using namespace General;

namespace DownloadManager {

namespace Daemon {

DownloadManagerFactory::DownloadManagerFactory(QObject *parent)
    : ManagerFactory(parent) {
}

BaseManager*
DownloadManagerFactory::createManager(System::Application* app,
                                    System::DBusConnection* connection,
                                    bool stoppable,
                                    QObject *parent) {
    return new DownloadManager(app, connection, stoppable, parent);
}

}  // Daemon

} // DownloadManager

} // Ubuntu
