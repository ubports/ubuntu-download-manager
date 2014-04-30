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

#ifndef UPLOADER_LIB_MANAGER_FACTORY_H
#define UPLOADER_LIB_MANAGER_FACTORY_H

#include <ubuntu/transfers/system/dbus_connection.h>
#include <ubuntu/transfers/system/application.h>
#include <ubuntu/transfers/manager_factory.h>

namespace Ubuntu {

using namespace Transfers;

namespace UploadManager {

namespace Daemon {

class UploadManagerFactory : public ManagerFactory {
    Q_OBJECT

 public:
    explicit UploadManagerFactory(QObject *parent = 0);

    virtual BaseManager* createManager(System::Application* app,
                                       System::DBusConnection* connection,
                                       bool stoppable = false,
                                       QObject *parent = 0);
};

}  // Daemon

}  // UploadManager

}  // Ubuntu

#endif
