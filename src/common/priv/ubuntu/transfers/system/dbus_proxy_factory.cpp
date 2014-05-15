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

#include <QDBusConnection>
#include "dbus_proxy_factory.h"

namespace {
    const QString DBUS_SERVICE_PATH = "org.freedesktop.DBus";
    const QString DBUS_OBJECT_PATH = "/";
}

namespace Ubuntu {

namespace Transfers {

namespace System {

DBusProxyFactory* DBusProxyFactory::_instance = nullptr;
QMutex DBusProxyFactory::_mutex;

DBusProxyFactory::DBusProxyFactory(QObject* parent)
    : QObject(parent) {
}

DBusProxy*
DBusProxyFactory::createDBusProxy(QObject* parent) {
    return new DBusProxy(DBUS_SERVICE_PATH, DBUS_OBJECT_PATH,
        QDBusConnection::sessionBus(), parent);
}

DBusProxy*
DBusProxyFactory::createDBusProxy(QSharedPointer<DBusConnection> connection,
                                  QObject* parent) {
    return new DBusProxy(DBUS_SERVICE_PATH, DBUS_OBJECT_PATH,
        connection->connection(), parent);
}

DBusProxyFactory*
DBusProxyFactory::instance() {
    if(_instance == nullptr) {
        _mutex.lock();
        if(_instance == nullptr)
            _instance = new DBusProxyFactory();
        _mutex.unlock();
    }
    return _instance;
}

void
DBusProxyFactory::setInstance(DBusProxyFactory* instance) {
    _instance = instance;
}

void
DBusProxyFactory::deleteInstance() {
    if(_instance != nullptr) {
        _mutex.lock();
        if(_instance != nullptr) {
            delete _instance;
            _instance = nullptr;
        }
        _mutex.unlock();
    }
}

}  // System

}  // Transfers

}  // Ubuntu
