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
    : DaemonTestCase("TestManager", parent) {
}

void
TestManager::init() {
    DaemonTestCase::init();
    _man = Manager::createSessionManager(daemonPath(), this);
}

void
TestManager::cleanup() {
    delete _man;
    DaemonTestCase::cleanup();
}

void
TestManager::testAllowMobileDataDownload_data() {
    QTest::addColumn<bool>("enabled");

    QTest::newRow("Enabled") << true;
    QTest::newRow("Disabled") << false;
}

void
TestManager::testAllowMobileDataDownload() {
    QFAIL("Not implemented");
}

void
TestManager::testAllowMobileDataDownloadError() {
    qDebug() << "THREAD ID =>>>" << QThread::currentThreadId();
    returnDBusErrors(true);
    _man->allowMobileDataDownload(true);
    QVERIFY(_man->isError());
}

void
TestManager::testIsMobileDataDownload() {
    QFAIL("Not implemented");
}

void
TestManager::testIsMobileDataDownloadError() {
    returnDBusErrors(true);
    QFAIL("Not implemented");
}

void
TestManager::testDefaultThrottle() {
    QFAIL("Not implemented");
}

void
TestManager::testDefaultThrottleError() {
    QFAIL("Not implemented");
}

void
TestManager::testSetDefaultThrottle_data() {
}

void
TestManager::testSetDefaultThrottle() {
    returnDBusErrors(true);
    _man->setDefaultThrottle(30);
    QVERIFY(_man->isError());
}

void
TestManager::testSetDefaultThrottleError() {
    returnDBusErrors(true);
    QFAIL("Not implemented");
}

void
TestManager::testExit() {
    QFAIL("Not implemented");
}

void
TestManager::testExitError() {
    returnDBusErrors(true);
    QFAIL("Not implemented");
}
