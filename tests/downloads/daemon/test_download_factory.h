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
#include <apparmor.h>
#include <process_factory.h>
#include <request_factory.h>
#include <system_network_info.h>

#include "uuid_factory.h"
#include "base_testcase.h"

using namespace Ubuntu::Transfers::System;
using namespace Ubuntu::Transfers::Tests;
using namespace Ubuntu::DownloadManager;
using namespace Ubuntu::DownloadManager::Daemon;

class TestDownloadFactory : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestDownloadFactory(QObject *parent = 0)
        : BaseTestCase("TestDownloadFactory", parent) {}

 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;

    void testCreateDownload();
    void testCreateDownloadWithHash();
    void testCreateMmsDownload();
    void testCreateGroupDownload();
    void testCreateDownloadWithValidUuid();
    void testCreateDownloadWithHashAndUuid();
    void testCreateGroupDownloadWithValidUuid();
    void testCreateDownloadForGroup();
    void testCreateDownloadForGroupWithHash();

 private:
    MockAppArmor* _apparmor;
    Factory* _downFactory;
};

#endif  // TEST_DOWNLOAD_FACTORY_H
