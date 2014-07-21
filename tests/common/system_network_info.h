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

#ifndef FAKE_SYSTEM_NETWORK_INFO_H
#define FAKE_SYSTEM_NETWORK_INFO_H

#include <QObject>
#include <ubuntu/transfers/system/system_network_info.h>
#include <gmock/gmock.h>

namespace Ubuntu {

namespace Transfers {

using namespace System;

namespace Tests {

class MockSystemNetworkInfo : public SystemNetworkInfo {
 public:
    explicit MockSystemNetworkInfo(QObject* parent = 0) :
        SystemNetworkInfo(parent) {}

    MOCK_METHOD0(currentNetworkMode, QNetworkInfo::NetworkMode());
    MOCK_METHOD0(isOnline, bool());

    using SystemNetworkInfo::cellIdChanged;
    using SystemNetworkInfo::currentCellDataTechnologyChanged;
    using SystemNetworkInfo::currentMobileCountryCodeChanged;
    using SystemNetworkInfo::currentMobileNetworkCodeChanged;
    using SystemNetworkInfo::currentNetworkModeChanged;
    using SystemNetworkInfo::locationAreaCodeChanged;
    using SystemNetworkInfo::networkInterfaceCountChanged;
    using SystemNetworkInfo::networkNameChanged;
    using SystemNetworkInfo::networkSignalStrengthChanged;
    using SystemNetworkInfo::networkStatusChanged;
    using SystemNetworkInfo::onlineStateChanged;
};

}  // Ubuntu

}  // Transfers

}  // Tests

#endif  // FAKE_SYSTEM_NETWORK_INFO_H
