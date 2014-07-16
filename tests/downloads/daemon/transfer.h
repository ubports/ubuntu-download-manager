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

#ifndef FAKE_TRANSFER_H
#define FAKE_TRANSFER_H

#include <ubuntu/transfers/transfer.h>
#include <gmock/gmock.h>

namespace Ubuntu {

namespace Transfers {

namespace Tests {

class MockTransfer : public Transfer {
 public:
    MockTransfer(const QString& id,
             const QString& path,
             bool isConfined,
             const QString& rootPath,
             QObject* parent = 0)
        : Transfer(id, "", path, isConfined, rootPath, parent) {}

    MOCK_CONST_METHOD0(transferId, QString());
    MOCK_CONST_METHOD0(path, QString());
    MOCK_CONST_METHOD0(isConfined, bool());
    MOCK_CONST_METHOD0(state, Transfer::State());
    MOCK_METHOD1(setState, void(Transfer::State));
    MOCK_METHOD0(canTransfer, bool());
    MOCK_CONST_METHOD0(isValid, bool());
    MOCK_CONST_METHOD0(addToQueue, bool());
    MOCK_METHOD1(setAddToQueue, void(bool));
    MOCK_METHOD0(pausable, bool());
    MOCK_METHOD0(cancelTransfer, void());
    MOCK_METHOD0(pauseTransfer, void());
    MOCK_METHOD0(resumeTransfer, void());
    MOCK_METHOD0(startTransfer, void());
    MOCK_METHOD1(setThrottle, void(qulonglong));
    MOCK_METHOD0(throttle, qulonglong());
    MOCK_METHOD1(allowGSMDownload, void(bool));
    MOCK_METHOD0(isGSMDownloadAllowed, bool());
    MOCK_METHOD0(cancel, void());
    MOCK_METHOD0(pause, void());
    MOCK_METHOD0(resume, void());
    MOCK_METHOD0(start, void());

    using Transfer::canceled;
    using Transfer::error;
    using Transfer::paused;
    using Transfer::resumed;
    using Transfer::started;
    using Transfer::stateChanged;
    using Transfer::throttleChanged;

};

} // Tests

} // Transfers

} // Ubuntu

#endif
