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

namespace {
    const QString& AUTH_PROCESS_KEY = "auth_process";
    const QString& AUTH_FILE_KEY = "auth_file";
    const QString& AUTH_PORT_NUMBER_KEY = "auth_port_number";
    const QString& NO_AUTH_PORT_NUMBER_KEY = "noauth_port_number";
    const QString& SPOOL_TEMP_KEY = "spool_temp";
    const QString& SQUID_TEMP_KEY = "squid_temp";
}

SquidConfTemplate::SquidConfTemplate(const QString& inFile)
        : _inFile(inFile) {
}

SquidConfTemplate::SquidConfTemplate(const QString& inFile, const QVariantMap& data)
    : SquidConfTemplate(inFile) {
    QStringList strProperties;
    strProperties << AUTH_PROCESS_KEY << AUTH_FILE_KEY << SPOOL_TEMP_KEY
        << SQUID_TEMP_KEY;

    foreach(const QString& key, strProperties) {
        if (data.contains(key)){
            setProperty(key.toStdString().c_str(), data[key]);
        }
    }

    QStringList intProperties;
    intProperties << AUTH_PORT_NUMBER_KEY << NO_AUTH_PORT_NUMBER_KEY;

    foreach(const QString& key, intProperties) {
        if (data.contains(key)){
            setProperty(key.toStdString().c_str(), data[key]);
        }
    }
}

//auth_process
void SquidConfTemplate::setAuthProcess(const QString& authProcess) {
    _authProcess = authProcess;
}

QString SquidConfTemplate::getAuthProcess() const {
    return _authProcess;
}

//auth_file
void SquidConfTemplate::setAuthFile(const QString& authFile) {
    _authFile = authFile;
}

QString SquidConfTemplate::getAuthFile() const {
    return _authFile;
}

// auth_port_number
void SquidConfTemplate::setAuthPortNumber(int authPortNumber) {
    _authPort = authPortNumber;
}

int SquidConfTemplate::getAuthPortNumber() const {
    return _authPort;
}

//noauth_port_number
void SquidConfTemplate::setNoAuthPortNumber(int noAuthPort) {
    _noAuthPort = noAuthPort;
}

int SquidConfTemplate::getNoAuthPortNumber() const {
    return _noAuthPort;
}

// spool_temp
void SquidConfTemplate::setSpoolTemp(const QString& spoolTemp) {
    _spoolTemp = spoolTemp;
}

QString SquidConfTemplate::getSpoolTemp() const {
    return _spoolTemp;
}

// squid_temp
void SquidConfTemplate::setSquidTemp(const QString& squidTemp) {
    _squidTemp = squidTemp;
}

QString SquidConfTemplate::getSquidTemp() const {
    return _squidTemp;
}

QString SquidConfTemplate::configure() {
    // ensure that all properties are set
    QStringList strProperties;
    strProperties << AUTH_PROCESS_KEY << AUTH_FILE_KEY << SPOOL_TEMP_KEY
        << SQUID_TEMP_KEY;

    foreach(const QString& key, strProperties) {
        auto value = property(key.toStdString().c_str()).toString();
        if (value.isEmpty()) {
            _isError = true;
            _errorString = QString("Value '$1' cannot be empty.").arg(key);
            return "";
        }
    }

    QStringList intProperties;
    intProperties << AUTH_PORT_NUMBER_KEY << NO_AUTH_PORT_NUMBER_KEY;

    foreach(const QString& key, intProperties) {
        auto value = property(key.toStdString().c_str()).toInt();
        if (value < 0) {
            _isError = true;
            _errorString = QString("Value '$1' cannot be negative.").arg(key);
            return "";
        }
    }

    // read the entire in file to memory (it is not large) and replace each
    // key for its value
    QFile f(_inFile);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            _isError = true;
            _errorString = "Could not open in configuration file.";
            return "";
    }

    QString data = f.readAll();

    foreach(const QString& key, strProperties) {
        auto value = property(key.toStdString().c_str()).toString();
        auto replaceKey = QString("${%1}").arg(key);
        data = data.replace(replaceKey, value);
    }

    foreach(const QString& key, intProperties) {
        auto value = property(key.toStdString().c_str()).toInt();
        auto replaceKey = QString("${%1}").arg(key);
        data = data.replace(replaceKey, QString::number(value));
    }

    return data;
}

bool SquidConfTemplate::isError() {
    return _isError;
}

QString SquidConfTemplate::error() {
    return _errorString;
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
