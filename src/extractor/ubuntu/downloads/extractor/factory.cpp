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

#include <ubuntu/downloads/extractor/deflator.h>
#include <ubuntu/downloads/extractor/factory.h>
#include <ubuntu/downloads/extractor/unzip.h>

namespace Ubuntu {

namespace DownloadManager {

namespace Extractor {

Factory::Factory(QObject* parent)
    : QObject(parent) {
}


Deflator*
Factory::deflator(const QString& id, const QString& path,
        const QString& destination) {
    Deflator* instance = nullptr;
    if (id == "unzip") {
        instance = new UnZip(path, destination);
    } else {
        _error = QString("Deflator '%1' type not known.").arg(id);
    }
    return instance;
}

bool
Factory::isError() const {
    return _error != QString::null;
}

QString
Factory::lastError() const {
    return _error;
}

}

}

}
