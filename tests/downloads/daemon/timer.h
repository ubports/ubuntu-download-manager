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

#ifndef FAKE_TIMER_H
#define FAKE_TIMER_H

#include <QObject>
#include <ubuntu/transfers/system/timer.h>
#include <gmock/gmock.h>

namespace Ubuntu {

namespace Transfers {

using namespace System;

namespace Tests {

class MockTimer : public Timer {
 public:
    MOCK_METHOD0(isActive, bool());
    MOCK_METHOD1(start, void(int));
    MOCK_METHOD0(stop, void());

    using Timer::timeout;
};

}  // Ubuntu

}  // Transfers

}  // Tests

#endif  // FAKE_TIMER_H
