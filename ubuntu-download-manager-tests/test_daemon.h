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

#ifndef TEST_DOWNLOAD_DAEMON_H
#define TEST_DOWNLOAD_DAEMON_H

#include <QObject>
#include <ubuntu/downloads/daemon.h>
#include <ubuntu/transfers/tests/base_testcase.h>
#include <ubuntu/transfers/tests/system/application.h>
#include <ubuntu/transfers/tests/system/dbus_connection.h>
#include <ubuntu/transfers/tests/system/timer.h>
#include <ubuntu/download_manager/tests/server/manager.h>

class TestDaemon : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestDaemon(QObject *parent = 0);

 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;
    void testStart();
    void testStartPath();
    void testStartFailServiceRegister();
    void testStartFailObjectRegister();
    void testTimerStop();
    void testTimerStart();
    void testTimeoutExit();
    void testDisableTimeout();
    void testSelfSignedCerts();
    void testSelfSignedCertsMissingPath();
    void testStoppable_data();
    void testStoppable();
    void testSetTimeout_data();
    void testSetTimeout();
    void testSetSelfSignedSslCerts();
};

#endif  // TEST_DOWNLOAD_DAEMON_H
