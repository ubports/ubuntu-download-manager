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
#include <QUrl>
#include <ubuntu/transfers/tests/base_testcase.h>
#include <ubuntu/download_manager/metatypes.h>
#include "testing_daemon.h"

using namespace Ubuntu::DownloadManager;
using namespace Ubuntu::Transfers::Tests;

class DaemonTestCase : public BaseTestCase {
    Q_OBJECT

 public:
    DaemonTestCase(const QString& testName,
                   QObject* parent = 0);
    DaemonTestCase(const QString& testName,
                   const QString& daemonProcess,
                   QObject* parent = 0);

    QString daemonPath();
    QUrl serverUrl();
    void addFileToHttpServer(const QString& absolutePath);
    void returnDBusErrors(bool errors);
    void returnAuthError(const QString &download, AuthErrorStruct error);
    void returnHttpError(const QString &download, HttpErrorStruct error);
    void returnNetworkError(const QString &download, NetworkErrorStruct error);
    void returnProcessError(const QString &download, ProcessErrorStruct error);

 private:
    void startUDMDaemon();
    void stopUDMDaemon();
    QString httpServerDir();
    void startHttpServer();
    void stopHttpServer();

 private slots:
    void onProcessError(QProcess::ProcessError error);

 protected slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;

 private:
    int _httpServerRetry = 0;
    int _httpServerRetryMax = 10;
    int _port = 8080;
    QString _daemonPath = "";
    QString _daemonExec = "";
    QString _httpServerDir = "";
    QProcess* _daemonProcess = nullptr;
    QProcess* _httpServer = nullptr;

};

#endif // DAEMON_TESTCASE_H
