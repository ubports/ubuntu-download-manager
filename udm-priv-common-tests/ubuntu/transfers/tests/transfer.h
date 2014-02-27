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
#include <ubuntu/transfers/tests/fake.h>

namespace Ubuntu {

namespace Transfers {

namespace Tests {

class FakeTransfer : public Transfer, public Fake {
    Q_OBJECT

 public:
    FakeTransfer(const QString& id,
                 const QString& path,
                 bool isConfined,
                 QObject* parent = 0)
        : Transfer(id, path, isConfined, parent) {}

    bool canTransfer() override;
    void setCanTransfer(bool canTransfer);
    void setThrottle(qulonglong speed) override;
    qulonglong throttle() override;
    void cancelTransfer();
    void pauseTransfer();
    void resumeTransfer();
    void startTransfer();
    bool pausable();

 private:
    bool _canTransfer;
};

} // Tests

} // Transfers

} // Ubuntu

#endif
