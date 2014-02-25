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
#include <ubuntu/download_manager/metatypes.h>
#include <glog/logging.h>
#include <iostream>
#include <sstream>

class QDBusError;
class QStringList;
class QSslError;
class QUrl;

#define TRACE DLOG(INFO) << __PRETTY_FUNCTION__

std::ostream& operator<<(std::ostream &out, const QString& var);
std::ostream& operator<<(std::ostream &out, const QByteArray& var);
std::ostream& operator<<(std::ostream &out, const QStringList& var);
std::ostream& operator<<(std::ostream &out, const QUrl& var);
std::ostream& operator<<(std::ostream &out, const QList<QSslError>& errors);
std::ostream& operator<<(std::ostream &out, const QDBusError& error);
std::ostream& operator<<(std::ostream &out, const QVariantMap& map);
std::ostream& operator<<(std::ostream &out, const QMap<QString, QString>& map);
std::ostream& operator<<(std::ostream &out, StructList list);

namespace Ubuntu {

namespace System {

class Logger : public QObject {
    Q_OBJECT

 public:

    static void setupLogging(const QString logDir= "");
    static bool setLogLevel(QtMsgType level);
    static QString getLogDir();
    static std::string toStdString(const QString& str);

 private:
    bool _initialized = false;
};

}  // System

}  // Ubuntu

#endif  // LOGGING_H
