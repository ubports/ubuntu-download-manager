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

#ifndef TEST_METADATA_H
#define TEST_METADATA_H

#include <QObject>
#include <ubuntu/transfers/metadata.h>
#include "base_testcase.h"

class TestMetadata : public BaseTestCase {
    Q_OBJECT

 public:
    explicit TestMetadata(QObject* parent = 0)
        : BaseTestCase("TestMetadata", parent) { }

 private slots:  // NOLINT(whitespace/indent)
    void testCommand_data();
    void testCommand();
    void testSetCommand_data();
    void testSetCommand();
    void testHasCommandTrue();
    void testHasCommandFalse();
    void testLocalPath_data();
    void testLocalPath();
    void testSetLocalPath_data();
    void testSetLocalPath();
    void testHasLocalPathTrue();
    void testHashLocalPathFalse();
    void testObjectPath_data();
    void testObjectPath();
    void testSetObjectPath_data();
    void testSetObjectPath();
    void testHasObjectPathTrue();
    void testHasObjectPathFalse();
    void testTitle_data();
    void testTitle();
    void testSetTitle_data();
    void testSetTitle();
    void testHasTitleTrue();
    void testHasTitleFalse();
    void testIndicator_data();
    void testIndicator();
    void testSetIndicator_data();
    void testSetIndicator();
    void testHasIndicatorTrue();
    void testHasIndicatorFasle();
    void testClick_data();
    void testClick();
    void testSetClick_data();
    void testSetClick();
    void testHasClickTrue();
    void testHasClickFalse();
    void testSetDeflate_data();
    void testSetDeflate();
    void testHasDeflateTrue();
    void testHasDeflateFalse();
    void testDownloadOwner_data();
    void testDownloadOwner();
    void testSetDownloadDestinationApp_data();
    void testSetDownloadDestinationApp();
    void testHasDownloadDestinationAppTrue();
    void testHasDownloadDestinationAppFalse();
    void testDestinationAppIsPickedFromEnv_data();
    void testDestinationAppIsPickedFromEnv();
    void testDestinationAppIsAlreadyPresent();
};

#endif
