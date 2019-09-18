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

#include <QFile>
#include <QProcess>
#include <QUrl>
#include <QVariant>

class SquidConfTemplate : public QObject {
    Q_OBJECT

    // use no very cpp names to simplify the init
    Q_PROPERTY(QString auth_process READ getAuthProcess WRITE setAuthProcess)
    Q_PROPERTY(QString auth_file READ getAuthFile WRITE setAuthFile)
    Q_PROPERTY(int auth_port_number READ getAuthPortNumber WRITE setAuthPortNumber)
    Q_PROPERTY(int noauth_port_number READ getNoAuthPortNumber WRITE setNoAuthPortNumber)
    Q_PROPERTY(QString spool_temp READ getSpoolTemp WRITE setSpoolTemp)
    Q_PROPERTY(QString squid_temp READ getSquidTemp WRITE setSquidTemp)

 public:
    SquidConfTemplate(const QString& inFile);
    SquidConfTemplate(const QString& inFile, const QVariantMap& data);
    void setAuthProcess(const QString& authProcess);
    QString getAuthProcess() const;
    void setAuthFile(const QString& authFile);
    QString getAuthFile() const;
    void setAuthPortNumber(int authPortNumber);
    int getAuthPortNumber() const;
    void setNoAuthPortNumber(int noAuthPort);
    int getNoAuthPortNumber() const;
    void setSpoolTemp(const QString& spoolTemp);
    QString getSpoolTemp() const;
    void setSquidTemp(const QString& squidTemp);
    QString getSquidTemp() const;
    QString configure();
    bool isError();
    QString error();

 private:
    bool _isError = false;
    QString _errorString;
    QString _inFile = "";
    QString _authProcess = "";
    QString _authFile = "";
    int _authPort = -1;
    int _noAuthPort = -1;
    QString _spoolTemp = "";
    QString _squidTemp = "";
};


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
    void error(const QString& error);

 private:
    QString getSquidExecutable();
    QString getAuthExecutable();
    QString getHtpasswdExecutable();
    QString getBaseDir();
    QString getSpoolTempPath();
    QString getSquidTempPath();
    QString getAuthTempPath();
    QString generateRandomString(int length);
    bool isSquidRunning(const QString& confFile);
    QString generateConfigFile();
    void generateSwap();
    void generateAuthFile();

 private:
    QString _confFile;
    QString _tmpPath;
    QString _dataPath;
    QString _usr;
    QString _pwd;
    QProcess* _squidProcess = nullptr;

};

#endif
