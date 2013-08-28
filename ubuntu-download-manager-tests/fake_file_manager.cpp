/*
 * Copyright 2013 2013 Canonical Ltd.
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

#include "./fake_file_manager.h"

FakeFileManager::FakeFileManager(QObject *parent)
    : FileManager(parent),
      Fake() {
}

bool
FakeFileManager::remove(const QString& path) {
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new StringWrapper(path));

        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("remove", params);
        _called.append(methodData);
    }
    return true;
}
