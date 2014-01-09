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

#include <QDebug>
#include <ubuntu/download_manager/system/dbus_connection.h>
#include <system/application.h>
#include <system/timer.h>
#include "testing_manager.h"
#include "testing_daemon.h"

TestingDaemon::TestingDaemon(QObject *parent)
    : Daemon(&TestingDaemon::createManager, parent) {
}

void
TestingDaemon::returnDBusErrors(bool errors) {
    TestingManager* man = qobject_cast<TestingManager*>(manager());
    man->returnDBusErrors(errors);
}

Manager*
TestingDaemon::createManager(System::Application* app,
                             System::DBusConnection* conn) {
    return new TestingManager(app, conn, true);
}
