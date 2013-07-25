/*
 * Copyright 2013 2013 Canonical Ltd.
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

#include <QDateTime>
#include <QDir>
#include <QStandardPaths>
#include "./xdg_basedir.h"
#include "./logger.h"


static void *_logger = NULL;

static void
_realMessageHandler(QtMsgType type,
                    const QMessageLogContext &context,
                    const QString &message) {
    if (_logger == NULL)
        return;

    Logger* log = reinterpret_cast<Logger*>(_logger);
    log->logMessage(type, context, message);
}

Logger::Logger(const QString filename) {
    if (filename == "") {
        _logFileName = getLogDir() + "/ubuntu-download-manager.log";
    } else {
        _logFileName = filename;
    }

    _logFile.setFileName(_logFileName);
    if (_logFile.open(QFile::WriteOnly | QFile::Truncate)) {
        _logStream.setDevice(&_logFile);
        _logStream.seek(0);
        _logStream.flush();
    }

    qInstallMessageHandler(_realMessageHandler);

    _initialized = true;
}

void
Logger::setupLogging(const QString filename) {
    if (_logger == NULL)
        _logger = new Logger(filename);
}

void
Logger::stopLogging() {
    if (_logger != NULL) {
        delete reinterpret_cast<Logger*>(_logger);
        _logger = NULL;
    }
}

bool
Logger::setLogLevel(QtMsgType level) {
    if (_logger != NULL) {
        Logger* log = reinterpret_cast<Logger*>(_logger);
        log->_logLevel = level;
        return true;
    }

    return false;
}

const QString
Logger::getMessageTypeString(QtMsgType type) {
    switch (type) {
        case QtDebugMsg:
            return QStringLiteral("DEBUG");
            break;
        case QtWarningMsg:
            return QStringLiteral("WARNING");
            break;
        case QtCriticalMsg:
            return QStringLiteral("CRITICAL");
            break;
        case QtFatalMsg:
            return QStringLiteral("FATAL");
            break;
    }
    return QStringLiteral("UNKNOWN");
}

QString
Logger::getLogDir() {
    QStringList pathComponents;
    pathComponents << "download_manager";
    return XDGBasedir::saveDataPath(pathComponents);
}

void
Logger::logMessage(QtMsgType type,
                   const QMessageLogContext &context,
                   const QString &message) {
    Q_UNUSED(context);
    if (type < _logLevel)
        return;

    QString logMessage;
    QTextStream _logMessage(&logMessage);
    _logMessage << QDateTime::currentDateTime().toString(
        _datetimeFormat).toUtf8().data()
        << " - " << getMessageTypeString(type).toUtf8().data()
        << " - " << message.toUtf8().data() << "\n";

    QTextStream _stdErr(stderr, QIODevice::WriteOnly);
    switch (type) {
        case QtDebugMsg:
        case QtCriticalMsg:
        case QtFatalMsg:
            _stdErr << logMessage;
            break;
        default:
            break;
    }
    _stdErr.device()->close();

    _logStream << logMessage;
    _logStream.flush();

    if (type == QtFatalMsg)
        abort();
}
