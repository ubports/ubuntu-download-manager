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

#ifndef UBUNTU_DOWNLOADMANAGER_CLIENT_LOGGER_H
#define UBUNTU_DOWNLOADMANAGER_CLIENT_LOGGER_H

#include <QMap>
#include <QMutex>
#include <QString>
#include <QStringList>
#include <QVariant>


namespace Ubuntu {

namespace DownloadManager {

class DownloadImpl;
class DownloadPCW;
class ManagerImpl;
class DownloadManagerPCW;
class DownloadsListManagerPCW;
class GroupManagerPCW;
class MetadataDownloadsListManagerPCW;
class DownloadStruct;

namespace Logging {

class LoggerPrivate;

/*!
    \class Logger
    \brief The Logger class allows to control a logging that
           is performed within the library allowed the developer
           to set the log level as well as the path of the file
           where the logs are written.
    \since 0.5

*/
class Logger {
    friend class Ubuntu::DownloadManager::DownloadImpl;
    friend class Ubuntu::DownloadManager::DownloadPCW;
    friend class Ubuntu::DownloadManager::ManagerImpl;
    friend class Ubuntu::DownloadManager::DownloadManagerPCW;
    friend class Ubuntu::DownloadManager::DownloadsListManagerPCW;
    friend class Ubuntu::DownloadManager::GroupManagerPCW;
    friend class Ubuntu::DownloadManager::MetadataDownloadsListManagerPCW;

 public:
    enum Level
    {
        Debug,
        Normal,
        Notification,
        Warning,
        Error,
        Critical
    };

    /*!
        \fn static void Logger::init(Level lvl, const QString& path)

        Initializes the logging service for the download library using
        the given \a level and writing the logs to the given \a path.
    */
    static void init(Level lvl, const QString& path);

 protected:
    static void log(Level lvl, const QString& msg);
    static void log(Level lvl, const QStringList& msg);
    static void log(Level lvl, const QString& msg, QMap<QString, QString> map);
    static void log(Level lvl, const QString& msg, DownloadStruct downStruct);

 private:
    static QMutex _mutex;
    static LoggerPrivate* _private;
};

}  // Logging

}  // DownloadManager

}  // Ubuntu

#endif
