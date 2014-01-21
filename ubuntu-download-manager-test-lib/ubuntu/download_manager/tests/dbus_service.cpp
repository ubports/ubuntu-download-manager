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

#include <QDir>
#include <QFile>
#include <QStringList>
#include <stdlib.h>
#include "dbus_service.h"

#define ADDRESS_KEY "@ADDRESS@"
#define DBUS_ADDRESS "unix:tmpdir=\"%1\""
#define CONFIGuRATION_IN "dbus-session.conf.in"
#define CONFIGuRATION_OUT "dbus-session.conf"

DBusService::DBusService(BaseTestCase* parent)
    : QObject(parent) {
}

DBusService::~DBusService() {
    delete _daemonProcess;
}

void
DBusService::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);
}

void
DBusService::onProcessError(QProcess::ProcessError errorCode) {
    emit error(QString("Error when launching daemon: %1").arg(errorCode));
}

bool
DBusService::generateConfigFile(QString& error) {
    auto test = qobject_cast<BaseTestCase*>(parent());
    // try to get the config gile, if it is not present
    // state the result as an error
    _dbusAddress = QString(DBUS_ADDRESS).arg(test->testDirectory());
    auto inPath = test->dataDirectory() + QDir::separator() + CONFIGuRATION_IN;
    auto outPath = test->testDirectory() + QDir::separator() + CONFIGuRATION_OUT;
    if (!QFile::exists(inPath)) {
        error = "DBus configuration in file could not be found in path '"
            + inPath + "'.";
        return false;
    }
    QFile inFile(inPath);
    auto opened = inFile.open(QFile::ReadOnly);

    if (!opened) {
        error = "Could not open file '" + inPath + "'.";
        return false;
    }

    auto confData = QString::fromUtf8(inFile.readAll());
    confData.replace(ADDRESS_KEY, _dbusAddress);

    QFile outFile(outPath);

    opened = outFile.open(QIODevice::ReadWrite | QFile::Append);

    if (!opened) {
        error = "";
        return false;
    }
    outFile.write(confData.toUtf8());
    return true;
}

void
DBusService::start() {
    QString errorMsg;
    auto generated = generateConfigFile(errorMsg);

    if (!generated) {
        emit error(errorMsg);
        return;
    }

    _daemonProcess = new QProcess();

    connect(_daemonProcess, static_cast<void(QProcess::*)
            (int, QProcess::ExitStatus)>(&QProcess::finished),
        this, &DBusService::onProcessFinished);
    connect(_daemonProcess, static_cast<void(QProcess::*)
            (QProcess::ProcessError)>(&QProcess::error),
        this, &DBusService::onProcessError);

    QStringList args;
    args << "--fork" << "--config-file=" << _configPath
        << "--print-address=1" << "--print-pid=2";

    _daemonProcess->start("dbus-daemon", args);
    // block until started to make sure that the daemon is ready for use
    _daemonProcess->waitForStarted();
    _dbusAddress = _daemonProcess->readAllStandardOutput().trimmed();
    _oldDBusAddress = qgetenv("DBUS_SESSION_BUS_ADDRESS").constData();
    qputenv("DBUS_SESSION_BUS_ADDRESS", _dbusAddress.toUtf8());
    emit started();
}

void
DBusService::stop() {
    if (_daemonProcess != nullptr) {
        _daemonProcess->kill();
        if (_oldDBusAddress.isEmpty()) {
            unsetenv("DBUS_SESSION_BUS_ADDRESS");
        } else {
            qputenv("DBUS_SESSION_BUS_ADDRESS", _oldDBusAddress.toUtf8());
        }
    }
    emit stopped();
}
