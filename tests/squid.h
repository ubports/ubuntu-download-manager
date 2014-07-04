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

#ifndef SQUID_SERVICE_H
#define SQUID_SERVICE_H

#include <QProcess>
#include <QUrl>

class SquidService : public QObject {
    Q_OBJECT

 public:
    SquidService(const QString& tmpPath,
                 const QString& dataPath,
                 QObject* parent=0); 

    QUrl proxyUrl();
    int proxyPort();
    QUrl authProxyUrl();
    int authProxyPort();
    QString username();
    QString password();

    void start();
    void stop();

 signals:
    void started(bool);
    void stopped(bool);

 private:
    QString getSquidExecutable();
    QString getAuthExecutable();
    QString getHtpasswdExecutable();
    QString getBaseDir();
    QString getSpoolTempPath();
    QString getSquidTempPath();
    QString getAuthTempPath();
    bool isSquidRunning();
    void generateConfigFile();

 private:
    QString _tmpPath;
    QString _dataPath;
    QProcess* _squidProcess = nullptr;

}; 

#endif
