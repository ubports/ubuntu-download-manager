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

#ifndef DAEMON_TESTCASE_H
#define DAEMON_TESTCASE_H

#include <QDebug>
#include <QObject>
#include <QProcess>
#include <downloads/daemon.h>  // comes from the priv lib, just for testing!!!!
#include "ubuntu/download_manager/tests/dbus_service.h"
#include "ubuntu/download_manager/tests/base_testcase.h"
#include "testing_daemon.h"

using namespace Ubuntu::DownloadManager;

class DaemonTestCase : public BaseTestCase {
    Q_OBJECT

 public:
    DaemonTestCase(const QString& testName,
                   QObject* parent = 0);
    DaemonTestCase(const QString& testName,
                   const QString& daemonProcess,
                   QObject* parent = 0);

    QString daemonPath();

 private slots:
    void onProcessError(QProcess::ProcessError error);

 protected slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;
    void returnDBusErrors(bool errors);

 private:
    QString _daemonPath;
    QString _daemonProcess;
    QProcess* _process = nullptr;
    DBusService* _service = nullptr;
    Daemon::Daemon* _daemon = nullptr;
};

#endif // DAEMON_TESTCASE_H
