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

#include "dbus_connection.h"

namespace Ubuntu {

namespace DownloadManager {

namespace System {

DBusConnection::DBusConnection(QObject* parent)
    : QObject(parent),
     _conn(QDBusConnection::connectToBus(QDBusConnection::ActivationBus, "DBUS")) {
}

bool
DBusConnection::registerService(const QString& serviceName) {
    return _conn.registerService(serviceName);
}

bool
DBusConnection::registerObject(const QString& path,
                    QObject* object,
                    QDBusConnection::RegisterOptions options) {
    return _conn.registerObject(path, object, options);
}

void
DBusConnection::unregisterObject(const QString& path,
                      QDBusConnection::UnregisterMode mode) {
    return _conn.unregisterObject(path, mode);
}

QDBusConnection
DBusConnection::connection() {
    return _conn;
}

}  // System

}  // DownloadManager

}  // Ubuntu
