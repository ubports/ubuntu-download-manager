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

#ifndef TEST_UPLOAD_FACTORY_H
#define TEST_UPLOAD_FACTORY_H

#include <QObject>
#include <ubuntu/uploads/factory.h>

#include "apparmor.h"
#include "base_testcase.h"
#include "process_factory.h"
#include "request_factory.h"
#include "system_network_info.h"
#include "uuid_factory.h"

using namespace Ubuntu::Transfers::System;
using namespace Ubuntu::Transfers::Tests;
using namespace Ubuntu::UploadManager;
using namespace Ubuntu::UploadManager::Daemon;

class TestUploadFactory : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestUploadFactory(QObject *parent = 0)
        : BaseTestCase("TestUploadFactory", parent) {}

 private slots:  // NOLINT(whitespace/indent)

    void init() override;
    void cleanup() override;
    void testCreateUpload();
    void testCreateMmsUpload();

 private:
    MockAppArmor* _apparmor;
    Factory* _upFactory;
};

#endif  // TEST_UPLOAD_FACTORY_H
