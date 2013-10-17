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

#include <unistd.h>
#include <sys/types.h>
#include <syslog.h>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include "logger.h"

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
    // decide if we are dealing with a system bus (that should use syslog)
    // or a session bus
    _isSystemBus = getuid() == 0;
    if (_isSystemBus) {
        openSyslogConnection();
    } else {
        openLogFile(filename);
    }

    qInstallMessageHandler(_realMessageHandler);

    _initialized = true;
}

void
Logger::openLogFile(const QString& filename) {
    if (filename == "") {
        _logFileName = getLogDir() + "/ubuntu-download-manager.log";
    } else {
        _logFileName = filename;
    }

    _logFile.setFileName(_logFileName);
    if (_logFile.open(QFile::WriteOnly | QFile::Append)) {
        _logStream.setDevice(&_logFile);
        _logStream.flush();
    }
}

void
Logger::openSyslogConnection() {
    openlog("ubuntu-download-manager",
        LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
}

void
Logger::setupLogging(const QString filename) {
    if (_logger == NULL)
        _logger = new Logger(filename);
}

void
Logger::stopLogging() {
    if (_logger != NULL) {
        Logger* log = reinterpret_cast<Logger*>(_logger);
        if (log->_isSystemBus) {
            closelog();
        } else {
            log->_logFile.close();
        }

        delete log;
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
    QString path = QStandardPaths::writableLocation(
        QStandardPaths::CacheLocation);
    qDebug() << "Logging dir is" << path;

    bool wasCreated = QDir().mkpath(path);
    if (!wasCreated) {
        qCritical() << "Could not create the logging path" << path;
    }
    return path;
}

void
Logger::logSessionMessage(const QString& message) {
    _logStream << message;
    _logStream.flush();
}

void
Logger::logSystemMessage(QtMsgType type, const QString& message) {
    const char* msg = message.toLatin1().data();
    // we using %s to avoid getting a compiler error when using
    // -Wall
    switch (type) {
        case QtDebugMsg:
            syslog(LOG_DEBUG, "%s", msg);
            break;
        case QtWarningMsg:
            syslog(LOG_WARNING, "%s", msg);
            break;
        case QtCriticalMsg:
            syslog(LOG_CRIT, "%s", msg);
            break;
        case QtFatalMsg:
            syslog(LOG_ALERT, "%s", msg);
            break;
        default:
            break;
    }
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

    if (_isSystemBus) {
        logSystemMessage(type, logMessage);
    } else {
        logSessionMessage(logMessage);
    }

    if (type == QtFatalMsg)
        abort();
}
