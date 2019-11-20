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

#include <QStandardPaths>
#include <QCoreApplication>
#include <QDebug>
#include <QStringList>
#include <QTimer>
#include "testing_daemon.h"
#define RETURN_ERRORS "-return-errors"
#define DAEMON_PATH "-daemon-path"

int main(int argc, char *argv[]) {
    QStandardPaths::setTestModeEnabled(true);
    QCoreApplication a(argc, argv);

    // similar to a real daemon but allows to force certain cases, for example,
    // returning dbus errors
    auto daemon = new TestingDaemon();
    QString daemonPath = "";

    // parse the args, if we have a --return-errors flag the daemon will always
    // return errors
    QStringList args = QCoreApplication::instance()->arguments();
    daemon->returnDBusErrors(args.contains(RETURN_ERRORS));

    auto index = args.indexOf(DAEMON_PATH);
    if (args.count() > index + 1) {
        daemonPath = args[index + 1];
        qDebug() << "Daemon will be started in path" << daemonPath;
        daemon->setDaemonPath(daemonPath);
    } else {
        qCritical() << "Missing daemon path";
    }

    // use a singleShot timer so that we start after exec so that exit works
    QTimer::singleShot(0, daemon, SLOT(start()));

    return a.exec();
}
