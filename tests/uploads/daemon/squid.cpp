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
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QTextStream>
#include <QThread>

#include <stdlib.h>
#include "squid.h"

namespace {
    const QString& NCSA_BASIC_PREFIX = "basic_";
    const QString& SQUID_PROCESS = "squid3";
    const QString& HTPASSWD_PROCESS = "htpasswd";
    const QString& AUTH_PROCESS_NAME = "ncsa_auth";
    const QString& AUTH_PROCESS_PATH = "/usr/lib/%1/";
    const QString& SQUID_CONFIG_FILE = "squid.conf.in";
    const QString& SQUID_DIR = "squid";
    const QString& SPOOL_DIR = "spool";
    const QString& AUTH_FILE = "htpasswd";
}

SquidService::SquidService(const QString& tmpPath,
                           const QString& dataPath,
                           QObject* parent)
    : QObject(parent),
      _tmpPath(tmpPath),
      _dataPath(dataPath) {
}

QUrl
SquidService::proxyUrl() {
    return QUrl();
}

int
SquidService::proxyPort() {
    return 0;
}

QUrl
SquidService::authProxyUrl() {
    return QUrl();
}

int
SquidService::authProxyPort() {
    return 0;
}

QString
SquidService::username() {
    return QString();
}

QString
SquidService::password() {
    return QString();
}

void
SquidService::start() {
    qDebug() << "Starting squid.";
    QStringList args;
    args << "-N" << "-X" << "-f";
    // generate the diff settings to be used with the test
    generateAuthFile();
    _confFile = generateConfigFile();
    generateSwap();
    _squidProcess = new QProcess();
    _squidProcess->start(SQUID_PROCESS, args);
    if (!isSquidRunning(_confFile)) {
        emit error("Could not start squid");
    }
}

void
SquidService::stop() {
    _squidProcess->kill();
    while(isSquidRunning(_confFile)) {}
}

QString
SquidService::getSquidExecutable() {
    return QStandardPaths::findExecutable(SQUID_PROCESS);
}

QString
SquidService::getAuthExecutable() {
    auto authPath = AUTH_PROCESS_PATH.arg("squid3");
    auto path = authPath + NCSA_BASIC_PREFIX + AUTH_PROCESS_NAME;
    QFileInfo fi(path);
    if (!fi.exists()) {
        path = authPath + AUTH_PROCESS_NAME;
    }
    qDebug() << "Auth executable location: '" << path << "'";
    return path;
}

QString
SquidService::getHtpasswdExecutable() {
    auto path = QStandardPaths::findExecutable(HTPASSWD_PROCESS);
    qDebug() << "Htpasswd executable: '" << path << "'"; 
    return path;
}

QString
SquidService::getBaseDir() {
    // join with the squid dir and make the path if needed
    auto squidPath = _tmpPath + "/" + SQUID_DIR;
    QFileInfo fi(squidPath);
    if (!fi.exists()) {
        QDir().mkpath(squidPath);
    }
    qDebug() << "Squid path '" << squidPath << "'";
    return squidPath;
}

QString
SquidService::getSpoolTempPath() {
    auto baseDir = getBaseDir();
    auto spoolPath = baseDir + "/" + SPOOL_DIR;
    QFileInfo fi(spoolPath);
    if (!fi.exists()) {
        QDir().mkpath(spoolPath);
    }
    qDebug() << "Spool path: '" << spoolPath << "'";
    return spoolPath;
}

QString
SquidService::getSquidTempPath() {
    auto baseDir = getBaseDir();
    auto squidPath = baseDir + "/" + SQUID_DIR;
    QFileInfo fi(squidPath);
    if (!fi.exists()) {
        QDir().mkpath(squidPath);
    }
    qDebug() << "Squid temp path: '" << squidPath << "'";
    return squidPath;
}

QString
SquidService::getAuthTempPath() {
    auto baseDir = getBaseDir();
    auto authPath = baseDir + "/" + AUTH_FILE;
    QFileInfo fi(authPath);
    if (!fi.exists(authPath)) {
        QDir().mkpath(authPath);
    }
    qDebug() << "Auth temp path: '" << authPath << "'";
    return authPath;
}

QString
SquidService::generateRandomString(int length) {
    static std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    std::string result;
    result.resize(length);
    srand(time(NULL));  // not very good practice but is ok for this use

    for (int i = 0; i < length; i++) {
        result[i] = charset[rand() % charset.length()];
    }
    auto qResult = QString::fromStdString(result);
    qDebug() << "Random string: '" << qResult << "'";
    return qResult;
}

bool
SquidService::isSquidRunning(const QString& confFile) {
    QStringList args;
    args << "-k" << "check" << "-f" << confFile;
    QScopedPointer<QProcess> process(new QProcess(this));
    QList<int> timeouts;
    timeouts << 0.4 << 0.1 << 0.1 << 0.2 << 0.5 << 1 << 3 << 5;
    foreach(int timeout, timeouts) {
        process->start(SQUID_PROCESS, args);
        if (process->waitForFinished()) {
            // check the result and either return true, any error means
            // that squid was not started
            if (process->exitStatus() == QProcess::NormalExit &&
                    process->exitCode() == 0) {
                qDebug() << "Squid is running.";
                return true;
            }
            else {
                qDebug() << "Squid not running retrying in " << timeout << ".";
                QThread::sleep(timeout);
            }  // exit was an error
        } else {
            process->kill();
            QThread::sleep(timeout);
        }  // took to long
    }
    return false;
}

QString
SquidService::generateConfigFile() {
    auto squidConfigPath = getBaseDir() + "/squid.conf";
    auto inSquidConfigPath = _dataPath + "/" + SQUID_CONFIG_FILE;
    // generate config file and write
    QVariantMap settings;
    settings[AUTH_PROCESS_KEY] = getAuthExecutable();
    settings[AUTH_FILE_KEY] = getAuthTempPath();
    settings[AUTH_PORT_NUMBER_KEY] = 8888;  // TODO: we might have issue if the port is used
    settings[NO_AUTH_PORT_NUMBER_KEY] = 8989;
    settings[SPOOL_TEMP_KEY] = getSpoolTempPath();
    settings[SQUID_TEMP_KEY] = getSquidTempPath();
    QScopedPointer<SquidConfTemplate> configTemplate(
        new SquidConfTemplate(inSquidConfigPath, settings));
    auto config = configTemplate->configure();
    if (configTemplate->isError()) {
        emit error(configTemplate->error());
        return "";
    }

    // write the config in the expected file
    QFile f(squidConfigPath);
    if (!f.open(QIODevice::ReadWrite | QIODevice::Text)) {
        emit error("Could not write squid conf.");
        return "";
    }

    QTextStream out(&f);
    out << config;
    f.close();
    qDebug() << "Squid configuration written to '" << squidConfigPath << "'";
    return squidConfigPath;
}

void
SquidService::generateSwap() {
    QStringList args;
    args << "-z" << "-f" << _confFile;
    QScopedPointer<QProcess> process(new QProcess(this));
    process->start(SQUID_PROCESS, args);
    process->waitForFinished();
    if (process->exitStatus() != QProcess::NormalExit ||
            process->exitCode() != 0) {
        emit error("Could not generate the swap dir.");
        return;
    }
}

void
SquidService::generateAuthFile() {
    // create the random usr and psw
    _usr = generateRandomString(10);
    _pwd = generateRandomString(10);
    auto authFile = getAuthTempPath();
    QStringList args;
    args << "-bc" << authFile << _usr << _pwd;
    auto authExec = getAuthExecutable();

    QScopedPointer<QProcess> process(new QProcess(this));
    process->start(authExec, args);
    process->waitForFinished();
    if (process->exitStatus() != QProcess::NormalExit ||
            process->exitCode() != 0) {
        emit error("Could not generate the auth file.");
        return;
    }
}
