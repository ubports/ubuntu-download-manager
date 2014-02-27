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

#ifndef TEST_DOWNLOAD_FACTORY_H
#define TEST_DOWNLOAD_FACTORY_H

#include <QObject>
#include <ubuntu/downloads/factory.h>
#include <ubuntu/transfers/tests/system/apparmor.h>
#include <ubuntu/transfers/tests/system/system_network_info.h>
#include <ubuntu/transfers/tests/system/request_factory.h>
#include <ubuntu/transfers/tests/system/process_factory.h>
#include <ubuntu/transfers/tests/system/uuid_factory.h>
#include <ubuntu/transfers/tests/base_testcase.h>
#include <ubuntu/transfers/tests/test_runner.h>

using namespace Ubuntu::Transfers::System;
using namespace Ubuntu::Transfers::Tests;
using namespace Ubuntu::DownloadManager;
using namespace Ubuntu::DownloadManager::Daemon;

class TestDownloadFactory : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestDownloadFactory(QObject *parent = 0);

 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;

    void testCreateDownload();
    void testCreateDownloadWithHash();
    void testCreateMmsDownload();
    void testCreateGroupDownload();
    void testCreateDownloadWithValidUuid();
    void testCreateDownloadWithNullUuid();
    void testCreateDownloadWithHashAndUuid();
    void testCreateDownloadWithHashAndNullUuid();
    void testCreateGroupDownloadWithValidUuid();
    void testCreateGroupDownloadWithNullUuid();
    void testCreateDownloadForGroup();
    void testCreateDownloadForGroupWithHash();

 private:
    FakeAppArmor* _apparmor;
    SystemNetworkInfo* _networkInfo;
    ProcessFactory* _processFactory;
    QSharedPointer<UuidFactory> _uuidFactory;
    Factory* _downFactory;
};

DECLARE_TEST(TestDownloadFactory)

#endif  // TEST_DOWNLOAD_FACTORY_H
