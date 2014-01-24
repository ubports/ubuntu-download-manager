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

#include "file_manager.h"

FakeFile::FakeFile(const QString& name)
    : File(name) {
}

QFile::FileError
FakeFile::error() const {
    if(_errWasSet) {
        return _err;
    } else {
        return File::error();
    }
}

bool
FakeFile::flush() {
    if(_errWasSet) {
        return false;
    } else {
        return File::flush();
    }
}

void
FakeFile::setError(QFile::FileError err) {
    _err = err;
    _errWasSet  = true;
}

FakeFileManager::FakeFileManager(QObject *parent)
    : FileManager(parent),
      Fake() {
}

File*
FakeFileManager::createFile(const QString& name) {
    if (_file == NULL) {
        return FileManager::createFile(name);
    } else {
        return _file;
    }
}

void
FakeFileManager::setFile(File* file) {
    _file = file;
}

bool
FakeFileManager::remove(const QString& path) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new StringWrapper(path, this));

        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("remove", params);
        _called.append(methodData);
    }
    return true;
}
