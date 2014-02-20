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

#include "download.h"
#include "error.h"
#include "download_list_impl.h"

namespace Ubuntu {

namespace DownloadManager {

DownloadListImpl::DownloadListImpl(QObject* parent)
    : DownloadList(parent) {
}

DownloadListImpl::DownloadListImpl(const QList<Download*> downs,
                                   QObject* parent)
    : DownloadList(parent),
      _downs(downs){
}

DownloadListImpl::DownloadListImpl(Error* err, QObject* parent)
    : DownloadList(parent),
      _lastError(err) {
}

DownloadListImpl::~DownloadListImpl() {
    delete _lastError;
    qDeleteAll(_downs);
}

QList<Download*>
DownloadListImpl::downloads() const {
    return _downs;
}

bool
DownloadListImpl::isError() const {
    return _lastError != nullptr;
}

Error*
DownloadListImpl::error() const {
    return _lastError;
}

}  // Ubuntu

}  // DownloadManager
