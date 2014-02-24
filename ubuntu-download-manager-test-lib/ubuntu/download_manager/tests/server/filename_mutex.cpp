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

#include "filename_mutex.h"

FakeFileNameMutex::FakeFileNameMutex(QObject* parent)
    : FileNameMutex(parent),
      Fake(){
}

QString
FakeFileNameMutex::lockFileName(const QString& expectedName) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new StringWrapper(expectedName, this));
        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);

        MethodData methodData("lockFileName", params);
        _called.append(methodData);
    }

    return FileNameMutex::lockFileName(expectedName);
}

void
FakeFileNameMutex::unlockFileName(const QString& filename) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new StringWrapper(filename, this));
        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);

        MethodData methodData("unlockFileName", params);
        _called.append(methodData);
    }
    FileNameMutex::unlockFileName(filename);
}

void
FakeFileNameMutex::clearMutex() {
    _paths.clear();
}
