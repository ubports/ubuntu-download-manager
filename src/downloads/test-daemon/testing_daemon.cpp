/*
 * Copyright 2013-2015 Canonical Ltd.
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
#include <ubuntu/transfers/system/dbus_connection.h>
#include <ubuntu/transfers/system/application.h>
#include <ubuntu/transfers/system/timer.h>
#include <ubuntu/downloads/download_adaptor_factory.h>
#include <ubuntu/transfers/manager_factory.h>
#include "testing_manager.h"
#include "testing_manager_adaptor.h"
#include "testing_daemon.h"

class TestingManagerFactory : public ManagerFactory {
 public:
    explicit TestingManagerFactory(QObject *parent = 0)
        : ManagerFactory(parent) {}

    BaseManager* createManager(Application* app,
                               DBusConnection* conn,
                               bool stoppable = false,
                               QObject *parent = 0) {
        Q_UNUSED(stoppable);
        return new TestingManager(app, conn, true, parent);
    }
};

TestingDaemon::TestingDaemon(QObject *parent)
    : BaseDaemon(new TestingManagerFactory(),
                 new DownloadAdaptorFactory(),
                 parent) {
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
TestingDaemon::returnHashError(const QString &download,
                               HashErrorStruct error) {
    qDebug() << "############################################";
    qDebug() << "returnHashError from error";
    qDebug() << "############################################";

    auto man = qobject_cast<TestingManager*>(manager());
    man->returnHashError(download, error);
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
TestingDaemon::start(const QString& path) {
    if (_path.isEmpty()) {
        BaseDaemon::start(path);
    } else {
        BaseDaemon::start(_path);
    }
    auto man = manager();
    TestingManager* manager = qobject_cast<TestingManager*>(man);
    _testsAdaptor = new TestingManagerAdaptor(manager);
}
