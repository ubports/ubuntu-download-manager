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

namespace {
    const QString& AUTH_PROCESS_KEY = "auth_process";
    const QString& AUTH_FILE_KEY = "auth_file";
    const QString& AUTH_PORT_NUMBER_KEY = "auth_port_number";
    const QString& NO_AUTH_PORT_NUMBER_KEY = "noauth_port_number";
    const QString& SPOOL_TEMP_KEY = "spool_temp";
    const QString& SQUID_TEMP_KEY = "squid_temp";
}

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
    SquidConfTemplate(const QString& inFile)
        : _inFile(inFile) {
    }

    SquidConfTemplate(const QString& inFile, const QVariantMap& data)
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
    void setAuthProcess(const QString& authProcess) {
        _authProcess = authProcess;
    }

    QString getAuthProcess() const {
        return _authProcess;
    }

    //auth_file
    void setAuthFile(const QString& authFile) {
        _authFile = authFile;
    }

    QString getAuthFile() const {
        return _authFile;
    }

    // auth_port_number
    void setAuthPortNumber(int authPortNumber) {
        _authPort = authPortNumber;
    }

    int getAuthPortNumber() const {
        return _authPort;
    }

    //noauth_port_number
    void setNoAuthPortNumber(int noAuthPort) {
        _noAuthPort = noAuthPort;
    }

    int getNoAuthPortNumber() const {
        return _noAuthPort;
    }

    // spool_temp
    void setSpoolTemp(const QString& spoolTemp) {
        _spoolTemp = spoolTemp;
    }

    QString getSpoolTemp() const {
        return _spoolTemp;
    }

    // squid_temp
    void setSquidTemp(const QString& squidTemp) {
        _squidTemp = squidTemp;
    }

    QString getSquidTemp() const {
        return _squidTemp;
    }

    QString configure() {
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

    bool isError() {
        return _isError;
    }

    QString error() {
        return _errorString;
    }

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
