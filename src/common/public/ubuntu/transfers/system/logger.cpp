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
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QDBusError>
#include <QMap>
#include <QStandardPaths>
#include <QStringList>
#include <QUrl>
#include <QVariantMap>
#include <QSslError>
#include <sys/types.h>
#include <unistd.h>
#include "logger.h"

std::ostream& operator<<(std::ostream &out, const QString& var) {
    out << " " << qPrintable(var);
    return out;
}

std::ostream& operator<<(std::ostream &out, const QByteArray& var) {
    out << " " << qPrintable(var);
    return out;
}

std::ostream& operator<<(std::ostream &out, const QStringList& var) {
    auto joined = var.join(" ");
    out << qPrintable(joined);
    return out;
}

std::ostream& operator<<(std::ostream &out, const QUrl& var) {
    out << " " << qPrintable(var.toString());
    return out;
}

std::ostream& operator<<(std::ostream &out, const QList<QSslError>& errors) {
    foreach(QSslError err, errors) {
        out << " " << err.errorString();
    }
    return out;
}

std::ostream& operator<<(std::ostream &out, const QDBusError& error) {
    out << " " << qPrintable(error.name())
        << ":" << qPrintable(error.message());
    return out;
}

std::ostream& operator<<(std::ostream &out, const QVariantMap& map) {
    out << "{";
    foreach(const QString& key, map.keys()) {
            out << key << ":" << map[key].toString();
    }
    out << "}";
    return out;
}

std::ostream& operator<<(std::ostream &out, const QMap<QString, QString>& map) {
    out << "{";
    foreach(const QString& key, map.keys()) {
            out << key << ":" << map[key];
    }
    out << "}";
    return out;
}

std::ostream& operator<<(std::ostream &out, const QList<QByteArray>& strs) {
    foreach(QByteArray str, strs) {
        out << " " << QString(str).toStdString();
    }
    return out;
}

namespace Ubuntu {

namespace Transfers {

namespace System {

static bool _init = false;

void
Logger::setupLogging(const QString logDir) {
    auto appName = QCoreApplication::instance()->applicationName();
    auto path = logDir;

    if (path == "" ){
        path = getLogDir() + QDir::separator() + appName + ".log";
    } else {
        bool wasCreated = QDir().mkpath(logDir);
        if (wasCreated) {
            path = QDir(logDir).absoluteFilePath(appName + ".log");
        } else {
            qCritical() << "Could not create the logging path" << logDir;
            path = getLogDir() + QDir::separator() + appName + ".log";
        }
    }

    if (!_init) {
        _init = true;
        auto pathStr = path.toStdString().c_str();

        google::InitGoogleLogging(appName.toStdString().c_str());
        google::SetLogDestination(google::ERROR, pathStr);
        google::SetLogDestination(google::WARNING, pathStr);
        google::SetLogDestination(google::INFO, pathStr);
        google::SetLogDestination(google::FATAL, pathStr);
    }
}

bool
Logger::setLogLevel(QtMsgType level) {
    Q_UNUSED(level);
    return false;
}

QString
Logger::getLogDir() {
    auto appName = QCoreApplication::instance()->applicationName();
    QString path = ""; 
    if (getuid() == 0) {
        path = "/var/log/" + appName;
    } else {
        path = QStandardPaths::writableLocation(
            QStandardPaths::CacheLocation);
    }
    qDebug() << "Logging dir is" << path;

    bool wasCreated = QDir().mkpath(path);
    if (!wasCreated) {
        qCritical() << "Could not create the logging path" << path;
    }
    return path;
}

std::string
Logger::toStdString(const QString& str) {
    // was added later in qt and we do not want to break ABI
    return str.toStdString();
}

}  // System

}  // Transfers

}  // Ubuntu
