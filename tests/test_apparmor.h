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

#ifndef TEST_APPARMOR_H
#define TEST_APPARMOR_H

#include <QObject>
#include "base_testcase.h"
#include "dbus_proxy_factory.h"

using namespace Ubuntu::Transfers::Tests;
using namespace Ubuntu::Transfers::System;

class TestAppArmor : public BaseTestCase {
    Q_OBJECT

public:
    explicit TestAppArmor(QObject *parent = 0)
        : BaseTestCase("TestAppArmor", parent) { }

 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;

    void testAppIdError();
    void testAppId();
    void testIsConfinedEmptyString();
    void testIsConfinedUnconfinedString();
    void testIsConfinedAppIdString();

 private:
    MockDBusProxyFactory* _dbusProxyFactory;
};

#endif // TEST_APN_REQUEST_FACTORY_H
