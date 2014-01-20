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

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QDBusError>
#include <QStandardPaths>
#include <QStringList>
#include <QUrl>
#include <QSslError>
#include <syslog.h>
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

namespace Ubuntu {

namespace DownloadManager {

namespace System {

static bool _init = false;

void
Logger::setupLogging(const QString filename) {
    auto path = filename;
    if (filename == "" ){
        path = getLogDir() + "/ubuntu-download-manager.log";
    }

    auto appName = QCoreApplication::instance()->applicationName();
    if (!_init) {
        _init = true;
        google::InitGoogleLogging(toStdString(appName).c_str());
    }
    google::SetLogDestination(google::INFO, toStdString(path).c_str());
}

bool
Logger::setLogLevel(QtMsgType level) {
    Q_UNUSED(level);
    return false;
}

QString
Logger::getLogDir() {
    QString path = ""; 
    if (getuid() == 0) {
        path = "/var/log/";
    } else {
        path = QStandardPaths::writableLocation(
            QStandardPaths::CacheLocation);
    }
    LOG(INFO) << "Logging dir is" << path;

    bool wasCreated = QDir().mkpath(path);
    if (!wasCreated) {
        LOG(ERROR) << "Could not create the logging path" << path;
    }
    return path;
}

std::string
Logger::toStdString(const QString& str) {
    std::string utf8_text = str.toUtf8().constData();
    return utf8_text;
}

}  // System

}  // DownloadManager

}  // Ubuntu
