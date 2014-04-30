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

#ifndef FAKE_FILENAME_MUTEX_H
#define FAKE_FILENAME_MUTEX_H

#include <ubuntu/transfers/system/filename_mutex.h>
#include <gmock/gmock.h>

namespace Ubuntu {

namespace Transfers {

using namespace System;

namespace Tests {

class MockFileNameMutex : public FileNameMutex {
 public:
    explicit MockFileNameMutex(QObject* parent = 0)
        : FileNameMutex(parent) {}
    MOCK_METHOD1(lockFileName, QString(const QString&));
    MOCK_METHOD1(unlockFileName, void(const QString&));
    MOCK_METHOD1(isLocked, bool(const QString&));
};

}  // Tests

}  // Transfers

}  // Ubuntu

#endif
