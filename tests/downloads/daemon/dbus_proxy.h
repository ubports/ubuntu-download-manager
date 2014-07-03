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

#ifndef FAKE_DBUS_PROXY_H
#define FAKE_DBUS_PROXY_H

#include <ubuntu/transfers/system/dbus_proxy.h>
#include <gmock/gmock.h>

namespace Ubuntu {

namespace Transfers {

using namespace System;

namespace Tests {

class MockDBusProxy : public DBusProxy {
 public:
    MockDBusProxy() : DBusProxy("", "", QDBusConnection::sessionBus()) {}

    MOCK_METHOD1(AddMatch, QDBusPendingReply<>(const QString&));
    MOCK_METHOD1(GetAdtAuditSessionData,
        QDBusPendingReply<QByteArray>(const QString&));
    MOCK_METHOD1(GetConnectionAppArmorSecurityContext,
        PendingReply<QString>*(const QString&));
    MOCK_METHOD1(GetConnectionSELinuxSecurityContext,
        QDBusPendingReply<QByteArray>(const QString&));
    MOCK_METHOD1(GetConnectionUnixProcessID,
        QDBusPendingReply<uint>(const QString&));
    MOCK_METHOD1(GetConnectionUnixUser,
        QDBusPendingReply<uint>(const QString&));
    MOCK_METHOD0(GetId, QDBusPendingReply<QString>());
    MOCK_METHOD1(GetNameOwner,
        QDBusPendingReply<QString>(const QString&));
    MOCK_METHOD0(Hello, QDBusPendingReply<QString>());
    MOCK_METHOD0(ListActivatableNames, QDBusPendingReply<QStringList>());
    MOCK_METHOD0(ListNames, QDBusPendingReply<QStringList>());
    MOCK_METHOD1(ListQueuedOwners,
        QDBusPendingReply<QStringList>(const QString&));
    MOCK_METHOD1(NameHasOwner, QDBusPendingReply<bool>(const QString&));
    MOCK_METHOD1(ReleaseName, QDBusPendingReply<uint>(const QString&));
    MOCK_METHOD0(ReloadConfig, QDBusPendingReply<>());
    MOCK_METHOD1(RemoveMatch, QDBusPendingReply<>(const QString&));
    MOCK_METHOD2(RequestName,
        QDBusPendingReply<uint>(const QString&, uint));
    MOCK_METHOD2(StartServiceByName,
        QDBusPendingReply<uint>(const QString&, uint));
    MOCK_METHOD1(UpdateActivationEnvironment,
        QDBusPendingReply<>(StringMap));
};

}  // Ubuntu

}  // Transfers

}  // Tests

#endif  // FAKE_DBUS_PROXY_H
