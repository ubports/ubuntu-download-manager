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

#ifndef UBUNTU_DOWNLOADS_MANAGER_FACTORY_H
#define UBUNTU_DOWNLOADS_MANAGER_FACTORY_H

#include <ubuntu/system/dbus_connection.h>
#include "ubuntu/system/application.h"
#include "ubuntu/general/manager_factory.h"

namespace Ubuntu {

using namespace General;

namespace DownloadManager {

namespace Daemon {

class DownloadManagerFactory : public ManagerFactory {
    Q_OBJECT

 public:
    explicit DownloadManagerFactory(QObject *parent = 0);

    virtual BaseManager* createManager(System::Application* app,
                                       System::DBusConnection* connection,
                                       bool stoppable = false,
                                       QObject *parent = 0);
};

}  // Daemon

}  // DownloadManager

}  // Ubuntu

#endif  // UBUNTU_GENERAL_MANAGER_FACTORY_H


