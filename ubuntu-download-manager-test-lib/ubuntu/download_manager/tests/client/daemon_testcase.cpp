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

#include "daemon_testcase.h"

DaemonTestCase::DaemonTestCase(const QString& testName, QObject* parent)
    : BaseTestCase(testName, parent) {
}


QString
DaemonTestCase::daemonPath() {
    return _daemonPath;
}

void
DaemonTestCase::init() {
    _dbusService = new DBusService(this);
    _dbusService->start();
    // WARNING: create a path for this exact test.. we might have
    // issues if we have to two object with the same name
    _daemonPath = "com.canonical.applications.testing.Downloader."
        + objectName();
    _daemon = new Daemon::Daemon(this);

    BaseTestCase::init();
    _daemon->enableTimeout(false);
    _daemon->start(_daemonPath);
}

void
DaemonTestCase::cleanup() {
    _daemon->stop();  // unregisters the service although the delete should do
                      // the same.. but I like to be explicit
    _dbusService->stop();

    delete _daemon;
    _daemon = nullptr;

    delete _dbusService;
    _dbusService = nullptr;

    BaseTestCase::cleanup();
}
