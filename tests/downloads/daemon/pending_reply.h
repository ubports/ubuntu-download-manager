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

#ifndef FAKE_PENDING_REPLY_H
#define FAKE_PENDING_REPLY_H

#include <ubuntu/transfers/system/pending_reply.h>
#include <gmock/gmock.h>

namespace Ubuntu {

namespace Transfers {

using namespace System;

namespace Tests {

template <class T>
class MockPendingReply : public PendingReply<T> {
 public:
    MOCK_CONST_METHOD0_T(value, T());
    MOCK_CONST_METHOD0_T(isError, bool());
    MOCK_CONST_METHOD0_T(error, QDBusError());
    MOCK_METHOD0_T(waitForFinished, void());
};

}  // Ubuntu

}  // Transfers

}  // Tests

#endif  // FAKE_DBUS_PROXY_H

