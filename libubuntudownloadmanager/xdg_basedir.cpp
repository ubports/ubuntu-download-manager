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

#include <QDebug>
#include <QDir>
#include <QFile>
#include <stdlib.h>
#include "./xdg_basedir.h"

// definitions of defaults
#define DEFAULT_CACHE_HOME ".cache"
#define DEFAULT_CONFIG_PATH "/etc/xdg"
#define DEFAULT_CONFIG_HOME ".config"
#define DEFAULT_DATA_PATH "/usr/local/share:/usr/share"
#define DEFAULT_DATA_HOME ".local/share"
#define XDG_CACHE_HOME "XDG_CACHE_HOME"
#define XDG_CONFIG_HOME "XDG_CONFIG_HOME"
#define XDG_DATA_HOME "XDG_DATA_HOME"
#define XDG_CONFIG_DIRS "XDG_CONFIG_DIRS"
#define XDG_DATA_DIRS "XDG_DATA_DIRS"

QString
XDGBasedir::cacheHome() {
    QString env_path = QString::fromUtf8(getenv(XDG_CACHE_HOME)).trimmed();
    if (env_path.isEmpty()) {
        QString defaultValue = QDir(QDir::homePath()).filePath(
                DEFAULT_CACHE_HOME);
        return defaultValue;
    }
    return env_path;
}

QString
XDGBasedir::configHome() {
    QString env_path = QString::fromUtf8(getenv(XDG_CONFIG_HOME)).trimmed();
    if (env_path.isEmpty()) {
        QString defaultValue = QDir(QDir::homePath()).filePath(
                DEFAULT_CONFIG_HOME);
        return defaultValue;
    }
    return env_path;
}

QString
XDGBasedir::dataHome() {
    QString env_path = QString::fromUtf8(getenv(XDG_DATA_HOME)).trimmed();
    if (env_path.isEmpty()) {
        QString defaultValue = QDir(QDir::homePath()).filePath(
                DEFAULT_DATA_HOME);
        return defaultValue;
    }
    return env_path;
}

QList<QString>
XDGBasedir::configDirs() {
    QList<QString> result;
    result.append(XDGBasedir::configHome());

    QString env_path = QString::fromUtf8(getenv(XDG_CONFIG_DIRS)).trimmed();
    if (env_path.isEmpty()) {
        return result;
    }

    QStringList paths = env_path.split(":");
    result.append(paths);

    return result;
}

QList<QString>
XDGBasedir::dataDirs() {
    QList<QString> result;
    result.append(XDGBasedir::dataHome());

    QString env_path = QString::fromUtf8(getenv(XDG_DATA_DIRS)).trimmed();
    if (env_path.isEmpty()) {
        return result;
    }

    QStringList paths = env_path.split(":");
    result.append(paths);
    return result;
}

QList<QString>
XDGBasedir::configPaths(QList<QString> resources) {
    QList<QString> result;
    QList<QString> configDirs = XDGBasedir::configDirs();
    foreach (QString configDir, configDirs) {  // NOLINT(whitespace/parens)
        QStringList pathComponents;
        pathComponents.append(configDir);
        pathComponents.append(resources);
        QString path = pathComponents.join(QDir::separator());
        if (QFile::exists(path)) {
            result.append(path);
        }
    }

    return result;
}

QList<QString>
XDGBasedir::dataPaths(QList<QString> resources) {
    QList<QString> result;
    QList<QString> dataDirs = XDGBasedir::dataDirs();
    foreach (QString dataDir, dataDirs) {  // NOLINT(whitespace/parens)
        QStringList pathComponents;
        pathComponents.append(dataDir);
        pathComponents.append(resources);
        QString path = pathComponents.join(QDir::separator());
        if (QFile::exists(path)) {
            result.append(path);
        }
    }
    return result;
}

QString
XDGBasedir::loadFirstConfig(QList<QString> resource) {
    QList<QString> configPaths = XDGBasedir::configPaths(resource);
    return configPaths.at(0);
}

QString
XDGBasedir::saveConfigPath(QList<QString> resources) {
    QString home = XDGBasedir::configHome();
    QStringList pathComponents;
    pathComponents.append(home);
    pathComponents.append(resources);
    QString path = pathComponents.join(QDir::separator());
    QFileInfo info = QFileInfo(path);
    if (!info.exists() || !info.isDir()) {
        QDir dir(home);
        QStringList child;
        child.append(resources);
        bool created = dir.mkpath(child.join(QDir::separator()));
        if (!created) {
            qCritical() << "Could not create path:"
                << child.join(QDir::separator());
            return QString("");
        }
    }
    return path;
}

QString
XDGBasedir::saveDataPath(QList<QString> resources) {
    QString home = XDGBasedir::dataHome();
    QStringList pathComponents;
    pathComponents.append(home);
    pathComponents.append(resources);
    QString path = pathComponents.join(QDir::separator());
    QFileInfo info = QFileInfo(path);
    if (!info.exists() || !info.isDir()) {
        QDir dir(home);
        QStringList child;
        child.append(resources);
        bool created = dir.mkpath(child.join(QDir::separator()));
        if (!created) {
            qCritical() << "Could not create path:"
                << child.join(QDir::separator());
            return QString("");
        }
    }
    return path;
}
