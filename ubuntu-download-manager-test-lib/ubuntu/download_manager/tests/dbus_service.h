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

#ifndef DBUS_SERVICE_H
#define DBUS_SERVICE_H

#include <QObject>
#include <QProcess>
#include "base_testcase.h"


class DBusService : public QObject {
    Q_OBJECT

 public:
    explicit DBusService(BaseTestCase* parent = 0);
    ~DBusService();

    void start();
    void stop();

 signals:
    void started();
    void stopped();
    void error(QString msg);

 private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

 private:
    bool generateConfigFile(QString& errorCode);

 private:
    QString _oldDBusAddress = "";
    QString _dbusAddress = "";
    QString _configPath = "";
    QProcess* _daemonProcess = nullptr;
};

#endif // DBUS_SERVICE_H
