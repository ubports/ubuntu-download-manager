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

#include <QObject>
#include <downloads/daemon.h>  // comes from the priv lib, just for testing!!!!
#include "ubuntu/download_manager/tests/base_testcase.h"

using namespace Ubuntu::DownloadManager;

class DaemonTestCase : public BaseTestCase {
    Q_OBJECT

 public:
    DaemonTestCase(const QString& testName, QObject *parent = 0);

    QString daemonPath();

 protected slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;

 private:
    QString _daemonPath;
    Daemon::Daemon* _daemon;
};

#endif // DAEMON_TESTCASE_H
