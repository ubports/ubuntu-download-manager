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
#include <QDBusError>
#include <ubuntu/download_manager/system/dbus_connection.h>
#include <ubuntu/system/application.h>
#include <ubuntu/system/timer.h>
#include "testing_manager.h"
#include "testing_manager_adaptor.h"
#include "testing_daemon.h"

TestingDaemon::TestingDaemon(QObject *parent)
    : Daemon(&TestingDaemon::createManager, parent) {
}

TestingDaemon::~TestingDaemon() {
    delete _testsAdaptor;
}

void
TestingDaemon::returnDBusErrors(bool errors) {
    auto man = qobject_cast<TestingManager*>(manager());
    man->returnDBusErrors(errors);
}

void
TestingDaemon::returnAuthError(const QString &download,
                               AuthErrorStruct error) {
    auto man = qobject_cast<TestingManager*>(manager());
    man->returnAuthError(download, error);
}

void
TestingDaemon::returnHttpError(const QString &download,
                               HttpErrorStruct error) {
    auto man = qobject_cast<TestingManager*>(manager());
    man->returnHttpError(download, error);
}

void
TestingDaemon::returnNetworkError(const QString &download,
                                  NetworkErrorStruct error) {
     auto man = qobject_cast<TestingManager*>(manager());
     man->returnNetworkError(download, error);
}

void
TestingDaemon::returnProcessError(const QString &download,
                                  ProcessErrorStruct error) {
    auto man = qobject_cast<TestingManager*>(manager());
    man->returnProcessError(download, error);
}

QString
TestingDaemon::daemonPath() {
    return _path;
}

void
TestingDaemon::setDaemonPath(QString path) {
    _path = path;
}

void
TestingDaemon::start(QString path) {
    if (_path.isEmpty()) {
        Daemon::start(path);
    } else {
        Daemon::start(_path);
    }
    auto man = manager();
    TestingManager* manager = qobject_cast<TestingManager*>(man);
    _testsAdaptor = new TestingManagerAdaptor(manager);
}

Manager*
TestingDaemon::createManager(System::Application* app,
                             System::DBusConnection* conn) {
    return new TestingManager(app, conn, true);
}
