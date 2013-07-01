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

#include "dbus_connection.h"

/*
 * PRIVATE IMPLEMENTATION
 */


class DBusConnectionPrivate
{
    Q_DECLARE_PUBLIC(DBusConnection)
public:
    explicit DBusConnectionPrivate(DBusConnection* parent);

    bool registerService(const QString& serviceName);
    bool registerObject(const QString& path, QObject* object,
        QDBusConnection::RegisterOptions options = QDBusConnection::ExportAdaptors);
    void unregisterObject(const QString& path,
        QDBusConnection::UnregisterMode mode = QDBusConnection::UnregisterNode);

private:
    QDBusConnection _conn;
    DBusConnection* q_ptr;

};

DBusConnectionPrivate::DBusConnectionPrivate(DBusConnection* parent):
    _conn(QDBusConnection::sessionBus()),
    q_ptr(parent)
{
}

bool DBusConnectionPrivate::registerService(const QString& serviceName)
{
    return _conn.registerService(serviceName);
}

bool DBusConnectionPrivate::registerObject(const QString& path, QObject* object,
    QDBusConnection::RegisterOptions options)
{
    return _conn.registerObject(path, object, options);
}

void DBusConnectionPrivate::unregisterObject(const QString& path, QDBusConnection::UnregisterMode mode)
{
    return _conn.unregisterObject(path, mode);
}

/*
 * PUBLIC IMPLEMENTATION
 */

DBusConnection::DBusConnection(QObject *parent) :
    QObject(parent),
    d_ptr(new DBusConnectionPrivate(this))
{
}

bool DBusConnection::registerService(const QString& serviceName)
{
    Q_D(DBusConnection);
    return d->registerService(serviceName);
}

bool DBusConnection::registerObject(const QString& path, QObject* object,
    QDBusConnection::RegisterOptions options)
{
    Q_D(DBusConnection);
    return d->registerObject(path, object, options);
}

void DBusConnection::unregisterObject(const QString& path, QDBusConnection::UnregisterMode mode)
{
    Q_D(DBusConnection);
    d->unregisterObject(path, mode);
}

