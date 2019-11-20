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

#include <ubuntu/transfers/system/file_manager.h>
#include <ubuntu/downloads/extractor/deflator.h>

namespace uts = Ubuntu::Transfers::System;

namespace Ubuntu {

namespace DownloadManager {

namespace Extractor {

Deflator::Deflator(const QString& path, const QString& destination,
        QObject* parent) :
    QObject(parent),
    _path(path),
    _destination(destination) {

    auto fm = uts::FileManager::instance();

    // we need to validate that the _path does exist and that it is a file
    // and that the destiation exists, but if it does not exist we can create
    // it
    if (!fm->exists(_path)) {
        _error = QString("File '%1' does not exist.").arg(_path);
    }

    if (fm->isDir(_path)) {
        _error = QString(
             "File '%1' cannot be extracted because is a dir.").arg(_path);
    }

    if (!fm->exists(_destination)) {
        // lets create the full path
    } else {
        // destination must be a dir
       if (!fm->isDir(_destination)) {
           _error = QString("Destination '%1' must be a dir.").arg(_destination);
       }
    }

}

bool
Deflator::isError() const {
    return _error != QString();
}

QString
Deflator::lastError() const {
    return _error;
}

void
Deflator::setLastError(const QString& error) {
    _error = error;
}

}  // Extractor

}  // DownloadManager

}  // Extrator
