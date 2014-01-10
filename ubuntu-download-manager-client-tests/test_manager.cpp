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

#include <QDebug>
#include "test_manager.h"

TestManager::TestManager(QObject *parent)
    : LocalTreeTestCase("TestManager", parent) {
}

void
TestManager::init() {
    LocalTreeTestCase::init();
    _man = Manager::createSessionManager(daemonPath(), this);
}

void
TestManager::cleanup() {
    LocalTreeTestCase::cleanup();
    delete _man;
}

void
TestManager::testAllowMobileDataDownload_data() {
    QTest::addColumn<bool>("enabled");

    QTest::newRow("Enabled") << true;
    QTest::newRow("Disabled") << false;
}

void
TestManager::testAllowMobileDataDownload() {
    QFETCH(bool, enabled);
    _man->allowMobileDataDownload(enabled);
    QCOMPARE(enabled, _man->isMobileDataDownload());
}

void
TestManager::testAllowMobileDataDownloadError() {
    returnDBusErrors(true);
    _man->allowMobileDataDownload(true);
    QVERIFY(_man->isError());
}

void
TestManager::testIsMobileDataDownloadError() {
    returnDBusErrors(true);
    _man->isMobileDataDownload();
    QVERIFY(_man->isError());
}

void
TestManager::testDefaultThrottleError() {
    returnDBusErrors(true);
    _man->defaultThrottle();
    QVERIFY(_man->isError());
}

void
TestManager::testSetDefaultThrottle_data() {
    QTest::addColumn<uint>("speed");

    QTest::newRow("First row") << 200u;
    QTest::newRow("Second row") << 1212u;
    QTest::newRow("Third row") << 998u;
    QTest::newRow("Last row") << 60u;
}

void
TestManager::testSetDefaultThrottle() {
    QFETCH(uint, speed);
    _man->setDefaultThrottle(speed);
    QCOMPARE(speed, (uint)_man->defaultThrottle());
}

void
TestManager::testSetDefaultThrottleError() {
    returnDBusErrors(true);
    _man->setDefaultThrottle(30);
    QVERIFY(_man->isError());
}

void
TestManager::testExitError() {
    returnDBusErrors(true);
    _man->exit();
    QVERIFY(_man->isError());
}
