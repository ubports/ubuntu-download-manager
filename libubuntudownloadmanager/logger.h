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

#ifndef DOWNLOADER_LIB_LOGGING_H
#define DOWNLOADER_LIB_LOGGING_H

#include <QFile>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <QtGlobal>


#ifdef TRACE
    #undef TRACE
    #define TRACE qDebug() << __FILE__ ":" << __LINE__ << __PRETTY_FUNCTION__
#else
    #define TRACE if (0) qDebug()
#endif


class Logger : public QObject {
    Q_OBJECT

 public:
    QtMsgType _logLevel = QtWarningMsg;

    explicit Logger(const QString filename = "");

    void logMessage(QtMsgType type,
                    const QMessageLogContext &context,
                    const QString &message);

    static void setupLogging(const QString filename = "");
    static void stopLogging();
    static bool setLogLevel(QtMsgType level);
    static const QString getMessageTypeString(QtMsgType type);
    static QString getLogDir();

 private:
    void openLogFile(const QString& filename);

    void openSyslogConnection();

    void logSessionMessage(const QString &message);

    void logSystemMessage(QtMsgType type, const QString &message);

 private:
    bool _isSystemBus = false;
    bool _initialized = false;
    QString _logFileName;
    QFile _logFile;
    QTextStream _logStream;

    const QString _datetimeFormat = "yyyy-MM-dd hh:mm:ss,zzz";
};


#endif  // LOGGING_H
