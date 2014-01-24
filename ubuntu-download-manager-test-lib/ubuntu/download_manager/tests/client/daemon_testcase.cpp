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

#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QDBusMessage>
#include <QScopedPointer>
#include <QProcessEnvironment>
#include "testing_interface.h"
#include "daemon_testcase.h"
#define TEST_DAEMON "ubuntu-download-manager-test-daemon"

DaemonTestCase::DaemonTestCase(const QString& testName,
                               QObject* parent)
    : BaseTestCase(testName, parent),
      _daemonProcess(TEST_DAEMON) {
}

DaemonTestCase::DaemonTestCase(const QString& testName,
                               const QString& daemonProcess,
                               QObject* parent)
    : BaseTestCase(testName, parent),
      _daemonProcess(daemonProcess) {
}


QString
DaemonTestCase::daemonPath() {
    return _daemonPath;
}

void
DaemonTestCase::onProcessError(QProcess::ProcessError error) {
    qDebug() << error << _process->readAllStandardError() << _process->readAllStandardOutput();
    QFAIL("Daemon process could not be started.");
}

void
DaemonTestCase::init() {
    BaseTestCase::init();
    // WARNING: create a path for this exact test.. we might have
    // issues if we have to two object with the same name
    _daemonPath = "com.canonical.applications.testing.Downloader."
        + objectName();
    _process = new QProcess();

    connect(_process, static_cast<void(QProcess::*)
        (QProcess::ProcessError)>(&QProcess::error), this,
        &DaemonTestCase::onProcessError);
    QStringList args;
    args << "-daemon-path" << _daemonPath << "-disable-timeout"
        << "-stoppable";
    _process->start(_daemonProcess, args);

    // loop until the service is registered
    auto conn = QDBusConnection::sessionBus();
    while(!conn.interface()->isServiceRegistered(_daemonPath));
}

void
DaemonTestCase::cleanup() {
    auto conn = QDBusConnection::sessionBus();
    returnDBusErrors(false);
    QDBusMessage exit = QDBusMessage::createMethodCall(_daemonPath,
        "/", "com.canonical.applications.DownloadManager", "exit");
    conn.send(exit);

    while(conn.interface()->isServiceRegistered(_daemonPath));
    _process->waitForFinished();

    delete _process;
    BaseTestCase::cleanup();
}

void
DaemonTestCase::returnDBusErrors(bool errors) {
    if (_process != nullptr) {
        auto conn = QDBusConnection::sessionBus();
        auto testingInterface = new TestingInterface(
            _daemonPath, "/", conn);
        QDBusPendingReply<> reply =
                testingInterface->returnDBusErrors(errors);
        reply.waitForFinished();

        if (reply.isError()) {
            delete testingInterface;
            QFAIL("Could not tell the daemon to return DBus errors.");
        }

        delete testingInterface;
    } else {
        QFAIL("returnDBusErrors must be used after init has been executed.");
    }
}
