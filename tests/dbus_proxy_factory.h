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

#ifndef FAKE_DBUS_PROXY_FACTORY_H
#define FAKE_DBUS_PROXY_FACTORY_H

#include <ubuntu/transfers/system/dbus_proxy_factory.h>
#include <gmock/gmock.h>

namespace Ubuntu {

namespace Transfers {

using namespace System;

namespace Tests {

class MockDBusProxyFactory : public DBusProxyFactory {
 public:
    MOCK_METHOD1(createDBusProxy, DBusProxy*(QObject*));
    MOCK_METHOD2(createDBusProxy,
        DBusProxy*(QSharedPointer<DBusConnection>, QObject*));
};

}  // Ubuntu

}  // Transfers

}  // Tests

#endif  // FAKE_DBUS_PROXY_FACTORY_H
