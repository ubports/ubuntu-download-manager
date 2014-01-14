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

#include "testing_file_download.h"

TestingFileDownload::TestingFileDownload(FileDownload* down,
                                         QObject* parent)
    : FileDownload(down->downloadId(),
                   down->path(),
                   down->isConfined(),
                   down->rootPath(),
                   down->url(),
                   down->metadata(),
                   down->headers(), parent),
      _down(down) {
}

TestingFileDownload::~TestingFileDownload() {
    delete _down;
}

void
TestingFileDownload::returnDBusErrors(bool errors) {
    _returnErrors = errors;
}

qulonglong
TestingFileDownload::progress() {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "progress");
    }
    return _down->progress();
}

qulonglong
TestingFileDownload::totalSize() {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "totalSize");
    }
    return _down->totalSize();
}

void
TestingFileDownload::setThrottle(qulonglong speed) {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "setThrottle");
    }
    _down->setThrottle(speed);
}
