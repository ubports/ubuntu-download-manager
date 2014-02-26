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

#ifndef UBUNTU_GENERAL_MANAGER_FACTORY_H
#define UBUNTU_GENERAL_MANAGER_FACTORY_H

#include <ubuntu/transfers/system/dbus_connection.h>
#include "ubuntu/transfers/system/application.h"
#include "base_manager.h"

namespace Ubuntu {

namespace General {

class ManagerFactory : public QObject {
 public:
    explicit ManagerFactory(QObject *parent = 0)
        : QObject(parent) {}

    virtual BaseManager* createManager(Application* app,
                                   DBusConnection* connection,
                                   bool stoppable = false,
                                   QObject *parent = 0) = 0;
};

}  // General

}  // Ubuntu

#endif  // UBUNTU_GENERAL_MANAGER_FACTORY_H

