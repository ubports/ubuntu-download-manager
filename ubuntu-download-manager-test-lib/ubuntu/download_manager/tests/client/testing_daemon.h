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

#ifndef TESTING_DAEMON_H
#define TESTING_DAEMON_H

#include <QObject>
#include <ubuntu/download_manager/metatypes.h>
#include <ubuntu/downloads/daemon.h>  // comes from the priv lib, just for testing!!!!

using namespace Ubuntu::Transfers;
using namespace Ubuntu::System;

class TestingManagerAdaptor;
class TestingDaemon : public Ubuntu::Transfers::BaseDaemon {
    Q_OBJECT
 public:
    explicit TestingDaemon(QObject *parent = 0);
    virtual ~TestingDaemon();

    // let the client test to tell the manager to return dbus errors
    void returnDBusErrors(bool errors);
    void returnAuthError(const QString &download, AuthErrorStruct error);
    void returnHttpError(const QString &download, HttpErrorStruct error);
    void returnNetworkError(const QString &download, NetworkErrorStruct error);
    void returnProcessError(const QString &download, ProcessErrorStruct error);

    QString daemonPath();
    void setDaemonPath(QString path);

 public slots:
    void start(const QString& path="com.canonical.applications.testing.Downloader") override;

 private:
    QString _path;
    TestingManagerAdaptor* _testsAdaptor = nullptr;

};

#endif // TESTING_DAEMON_H
