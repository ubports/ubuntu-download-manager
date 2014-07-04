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
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
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
    std::string SQUID_START_ARGS[] = {"-N", "-X", "-f"};
    const QString& PROXY_ENV_VAR = "SQUID_PROXY_SETTINGS";
}

class SquidConfTemplate {
 public:
    SquidConfTemplate(const QString& inFile) {
    }

    //auth_process
    void setauth_process

    //auth_file
    void setauth_file
    // auth_port_number
    void setauth_port_number

    //noauth_port_number
    void setnoauth_port_number

    // spool_temp
    void setspool_temp

    // squid_temp
    void setsquid_temp

    //spool_temp
    void setspool_temp
 private:
    QString _inFile;
};

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
}

void
SquidService::stop() {
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
    return path;
}

QString
SquidService::getHtpasswdExecutable() {
    return QStandardPaths::findExecutable(HTPASSWD_PROCESS);
}

QString
SquidService::getBaseDir() {
    // join with the squid dir and make the path if needed
    auto squidPath = _tmpPath + "/" + SQUID_DIR;
    QFileInfo fi(squidPath);
    if (!fi.exists()) {
        QDir().mkpath(squidPath);
    }
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
    return authPath;
}

bool
SquidService::isSquidRunning() {
    return false;
}

void
SquidService::generateConfigFile() {
    auto squidConfigPath = getBaseDir() + "/squid.conf";
    auto inSquidConfigPath = _dataPath + "/" + SQUID_CONFIG_FILE;
    // generate config file and write
}
