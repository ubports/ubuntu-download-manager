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

#ifndef FAKE_PROCESS_H
#define FAKE_PROCESS_H

#include <QObject>
#include <ubuntu/transfers/system/process.h>
#include <gmock/gmock.h>

namespace Ubuntu {

namespace Transfers {

using namespace System;

namespace Tests {

class MockProcess : public Process {
 public:
    explicit MockProcess(QObject *parent = 0)
        : Process(parent) {}

    MOCK_METHOD3(start,
        void(const QString&, const QStringList&, QProcess::OpenMode));
    MOCK_CONST_METHOD0(arguments, QStringList());
    MOCK_CONST_METHOD0(program, QString());
    MOCK_METHOD0(readAllStandardOutput, QByteArray());
    MOCK_METHOD0(readAllStandardError, QByteArray());

    using Process:finished;
    using Process::error;
};

}  // Ubuntu

}  // Transfers

}  // Tests

#endif  // FAKE_PROCESS_H
