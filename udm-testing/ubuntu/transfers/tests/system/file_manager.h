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
#include <ubuntu/transfers/tests/fake.h>

namespace Ubuntu {

namespace Transfers {

using namespace System;

namespace Tests {

class FakeFile : public File {
    Q_OBJECT

 public:
    explicit FakeFile(const QString& name);
    QFile::FileError error() const override;
    bool flush() override;

    void setError(QFile::FileError err);

 private:
    bool _errWasSet = false;
    QFile::FileError _err;
};

class FakeFileManager : public FileManager, public Fake {
    Q_OBJECT

 public:
    explicit FakeFileManager(QObject *parent = 0);

    File* createFile(const QString& name) override;
    void setFile(File* file);
    bool remove(const QString& path) override;

 private:
    File* _file = nullptr;
};

}  // Ubuntu

}  // Transfers

}  // Tests

#endif  // FAKE_FILE_MANAGER_H
