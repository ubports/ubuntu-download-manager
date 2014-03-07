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

#include <QDebug>
#include "transfer.h"

namespace Ubuntu {

namespace Transfers {

namespace Tests {

bool
FakeTransfer::canTransfer() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("canTransfer");
        _called.append(methodData);
    }
    return _canTransfer;
}

void
FakeTransfer::setCanTransfer(bool canTransfer) {
    _canTransfer = canTransfer;
}

void
FakeTransfer::setThrottle(qulonglong speed) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new UintWrapper(speed, this));

        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("setThrottle", params);
        _called.append(methodData);
    }
    Transfer::setThrottle(speed);
}

qulonglong
FakeTransfer::throttle() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("throttle");
        _called.append(methodData);
    }
    return Transfer::throttle();
}

void
FakeTransfer::cancelTransfer() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("cancelTransfer");
        _called.append(methodData);
    }
}

void
FakeTransfer::pauseTransfer() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("pauseTransfer");
        _called.append(methodData);
    }
}

void
FakeTransfer::resumeTransfer() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("resumeTransfer");
        _called.append(methodData);
    }
}

void
FakeTransfer::startTransfer() {
    if (_recording) {
        MethodData methodData;
        methodData.setMethodName("startTransfer");
        _called.append(methodData);
    }
}

bool
FakeTransfer::pausable() {
    return true;
}

} // Tests

} // Transfers

} // Ubuntu
