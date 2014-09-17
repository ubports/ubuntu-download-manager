/*
 * Copyright 2013-2014 Canonical Ltd.
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

#ifndef DOWNLOADER_LIB_DBUS_CONNECTION_H
#define DOWNLOADER_LIB_DBUS_CONNECTION_H

#include <QMutex>
#include <QObject>
#include <QtDBus/QDBusConnection>

namespace Ubuntu {

namespace Transfers {

namespace System {

class DBusConnection : public QObject {
    Q_OBJECT
 public:

    virtual bool registerService(const QString& serviceName);
    virtual bool unregisterService(const QString& serviceName);
    virtual bool registerObject(const QString& path, QObject* object,
        QDBusConnection::RegisterOptions options = QDBusConnection::ExportAdaptors);  // NOLINT(whitespace/line_length)
    virtual void unregisterObject(const QString& path,
        QDBusConnection::UnregisterMode mode = QDBusConnection::UnregisterNode);
    virtual bool send(const QDBusMessage& message) const;
    virtual QDBusConnection connection();

    // we will only have a single connection in the system
    static DBusConnection* instance();
    static DBusConnection* instance(QDBusConnection conn);

    // only used for testing purposes
    static void setInstance(DBusConnection* instance);
    static void deleteInstance();

 protected:
    explicit DBusConnection(QObject* parent = 0);
    DBusConnection(QDBusConnection conn, QObject* parent = 0);

 private:
    // singleton vars
    static DBusConnection* _instance;
    static QMutex _mutex;

    // instance vars
    QDBusConnection _conn;
};

}  // System

}  // Transfers

}  // Ubuntu

#endif  // DOWNLOADER_LIB_DBUS_CONNECTION_H
