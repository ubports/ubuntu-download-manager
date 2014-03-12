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

#ifndef FAKE_FILE_MANAGER_H
#define FAKE_FILE_MANAGER_H

#include <QObject>
#include <ubuntu/transfers/system/file_manager.h>
#include <gmock/gmock.h>

namespace Ubuntu {

namespace Transfers {

using namespace System;

namespace Tests {

class MockFile : public File {
 public:
    explicit MockFile(const QString& name)
        : File(name) {}

    MOCK_METHOD0(error, QFile::FileError());
    MOCK_METHOD0(flush, bool());
};

class MockFileManager : public FileManager {
 public:
    explicit MockFileManager(QObject *parent = 0)
        : FileManager(parent) {}

    MOCK_METHOD1(createFile, File*(const QString&));
    MOCK_METHOD1(remove, bool(const QString&));
};

}  // Ubuntu

}  // Transfers

}  // Tests

#endif  // FAKE_FILE_MANAGER_H
