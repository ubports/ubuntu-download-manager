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

#ifndef TEST_DOWNLOAD_FACTORY_H
#define TEST_DOWNLOAD_FACTORY_H

#include <QObject>
#include <downloads/factory.h>
#include "./fake_apparmor.h"
#include "./fake_system_network_info.h"
#include "./fake_request_factory.h"
#include "./fake_process_factory.h"
#include "./fake_uuid_factory.h"
#include "./base_testcase.h"
#include "./test_runner.h"

using namespace Ubuntu::DownloadManager;

class TestDownloadFactory : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestDownloadFactory(QObject *parent = 0);

 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;

    void testCreateDownload();
    void testCreateDownloadWithHash();
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
    QSharedPointer<SystemNetworkInfo> _networkInfo;
    QSharedPointer<RequestFactory> _nam;
    QSharedPointer<ProcessFactory> _processFactory;
    QSharedPointer<UuidFactory> _uuidFactory;
    Factory* _downFactory;
};

DECLARE_TEST(TestDownloadFactory)

#endif  // TEST_DOWNLOAD_FACTORY_H
