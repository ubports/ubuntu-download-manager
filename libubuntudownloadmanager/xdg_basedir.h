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

#ifndef DOWNLOADER_LIB_XDG_BASEDIR_H
#define DOWNLOADER_LIB_XDG_BASEDIR_H

#include <QList>
#include <QObject>
#include <QString>
#include <stdio.h>
#include <string>

class XDGBasedir : public QObject {
    Q_OBJECT

 public:
    static QString cacheHome();
    static QString configHome();
    static QString dataHome();
    static QList<QString> configDirs();
    static QList<QString> dataDirs();
    static QList<QString> configPaths(QList<QString> resource);
    static QList<QString> dataPaths(QList<QString> resource);
    static QString loadFirstConfig(QList<QString> resource);
    static QString saveConfigPath(QList<QString> resource);
    static QString saveDataPath(QList<QString> resource);

 private:
    static QString DEFAULT_CACHE_HOME;
    static QString DEFAULT_CONFIG_HOME;
    static QString DEFAULT_CONFIG_PATH;
    static QString DEFAULT_DATA_PATH;
    static QString DEFAULT_DATA_HOME;
    static std::string XDG_CACHE_HOME;
    static std::string XDG_CONFIG_HOME;
    static std::string XDG_DATA_HOME;
    static std::string XDG_CONFIG_DIRS;
    static std::string XDG_DATA_DIRS;
};

#endif  // DOWNLOADER_LIB_XDG_BASEDIR_H
