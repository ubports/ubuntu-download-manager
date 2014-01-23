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

#ifndef TESTING_DAEMON_H
#define TESTING_DAEMON_H

#include <QDBusError>
#include <QObject>
#include <downloads/daemon.h>  // comes from the priv lib, just for testing!!!!
#include "testing_manager_adaptor.h"

class TestingDaemon : public Ubuntu::DownloadManager::Daemon::Daemon {
    Q_OBJECT
 public:
    explicit TestingDaemon(QObject *parent = 0);
    virtual ~TestingDaemon();

    // let the client test to tell the manager to return dbus errors
    void returnDBusErrors(bool errors);

    QString daemonPath();
    void setDaemonPath(QString path);

 public slots:
    void start(QString path="com.canonical.applications.testing.Downloader") override;

 private:
    static Ubuntu::DownloadManager::Daemon::Manager* createManager(
         Ubuntu::DownloadManager::System::Application* app,
         Ubuntu::DownloadManager::System::DBusConnection* conn);

    QString _path;
    TestingManagerAdaptor* _testsAdaptor = NULL;

};

#endif // TESTING_DAEMON_H
