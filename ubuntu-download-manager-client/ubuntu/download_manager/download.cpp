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

#include "error.h"
#include "download.h"

namespace Ubuntu {

namespace DownloadManager {

Download::Download(Error* err, QObject* parent)
    : QObject(parent),
      _error(err) {
    // TODO(mandel): complete implementation
    Q_UNUSED(err);
}

Download::Download(QDBusObjectPath path, QObject* parent)
    : QObject(parent) {
    // TODO(mandel): complete implementation
    Q_UNUSED(path);
}

Download::~Download() {
    delete _error;
}

bool
Download::isError() {
    // TODO(mandel): complete implementation
    return false;
}

Error*
Download::error() {
    // TODO(mandel): complete implementation
    return NULL;
}

}  // DownloadManager

}  // Ubuntu
