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

#include <QDBusMessage>
#include <QScopedPointer>
#include <ubuntu/transfers/system/apparmor.h>

#include "dbus_connection.h"
#include "dbus_proxy.h"
#include "pending_reply.h"
#include "test_apparmor.h"

using ::testing::_;
using ::testing::ByRef;
using ::testing::Mock;
using ::testing::Return;

void
TestAppArmor::init() {
    BaseTestCase::init();
    _dbusProxyFactory = new MockDBusProxyFactory();
    DBusProxyFactory::setInstance(_dbusProxyFactory );
}

void
TestAppArmor::cleanup() {
    BaseTestCase::cleanup();
    DBusProxyFactory::deleteInstance();
}

void
TestAppArmor::testAppIdError() {
    QString caller = "my app";
    auto dbusProxy = new MockDBusProxy();
    auto conn = new MockDBusConnection();
    auto reply = new MockPendingReply<QString>();

    EXPECT_CALL(*_dbusProxyFactory, createDBusProxy(conn, _))
        .Times(1)
        .WillOnce(Return(dbusProxy));

    EXPECT_CALL(*dbusProxy, GetConnectionAppArmorSecurityContext(caller))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, waitForFinished())
        .Times(1);

    EXPECT_CALL(*reply, isError())
        .Times(1)
        .WillOnce(Return(true));

    QScopedPointer<AppArmor> appArmor(new AppArmor(conn));

    auto appId = appArmor->appId(caller);
    QCOMPARE(QString(), appId);

    QVERIFY(Mock::VerifyAndClearExpectations(dbusProxy));
    QVERIFY(Mock::VerifyAndClearExpectations(_dbusProxyFactory));
}

void
TestAppArmor::testAppId() {
    QString caller = "my app";
    QString expectedAppId = "APPID";
    auto dbusProxy = new MockDBusProxy();
    auto conn = new MockDBusConnection();
    auto reply = new MockPendingReply<QString>();

    EXPECT_CALL(*_dbusProxyFactory, createDBusProxy(conn, _))
        .Times(1)
        .WillOnce(Return(dbusProxy));

    EXPECT_CALL(*dbusProxy, GetConnectionAppArmorSecurityContext(caller))
        .Times(1)
        .WillOnce(Return(reply));

    EXPECT_CALL(*reply, waitForFinished())
        .Times(1);

    EXPECT_CALL(*reply, isError())
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(*reply, value())
        .Times(1)
        .WillOnce(Return(expectedAppId));

    QScopedPointer<AppArmor> appArmor(new AppArmor(conn));

    auto appId = appArmor->appId(caller);
    QCOMPARE(expectedAppId, appId);

    QVERIFY(Mock::VerifyAndClearExpectations(dbusProxy));
    QVERIFY(Mock::VerifyAndClearExpectations(_dbusProxyFactory));
}

void
TestAppArmor::testIsConfinedEmptyString() {
    QString caller = "";
    auto dbusProxy = new MockDBusProxy();
    auto conn = new MockDBusConnection();

    EXPECT_CALL(*_dbusProxyFactory, createDBusProxy(conn, _))
        .Times(1)
        .WillOnce(Return(dbusProxy));

    QScopedPointer<AppArmor> appArmor(new AppArmor(conn));
    bool isConfined = appArmor->isConfined(caller);

    QVERIFY(!isConfined);

    QVERIFY(Mock::VerifyAndClearExpectations(dbusProxy));
    QVERIFY(Mock::VerifyAndClearExpectations(_dbusProxyFactory));
}

void
TestAppArmor::testIsConfinedUnconfinedString() {
    auto dbusProxy = new MockDBusProxy();
    auto conn = new MockDBusConnection();

    EXPECT_CALL(*_dbusProxyFactory, createDBusProxy(conn, _))
        .Times(1)
        .WillOnce(Return(dbusProxy));

    QScopedPointer<AppArmor> appArmor(new AppArmor(conn));
    bool isConfined = appArmor->isConfined(AppArmor::UNCONFINED_ID);

    QVERIFY(!isConfined);

    QVERIFY(Mock::VerifyAndClearExpectations(dbusProxy));
    QVERIFY(Mock::VerifyAndClearExpectations(_dbusProxyFactory));
}

void
TestAppArmor::testIsConfinedAppIdString() {
    QString caller = "APPID";
    auto dbusProxy = new MockDBusProxy();
    auto conn = new MockDBusConnection();

    EXPECT_CALL(*_dbusProxyFactory, createDBusProxy(conn, _))
        .Times(1)
        .WillOnce(Return(dbusProxy));

    QScopedPointer<AppArmor> appArmor(new AppArmor(conn));
    bool isConfined = appArmor->isConfined(caller);

    QVERIFY(isConfined);

    QVERIFY(Mock::VerifyAndClearExpectations(dbusProxy));
    QVERIFY(Mock::VerifyAndClearExpectations(_dbusProxyFactory));
}

QTEST_MAIN(TestAppArmor)
