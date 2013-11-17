/*
 * Copyright 2013 Canonical Ltd.
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

#include "sm_file_download.h"

namespace Ubuntu {

namespace DownloadManager {

SMFileDownload::SMFileDownload(QObject *parent)
    : QObject(parent) {
}

void
SMFileDownload::emitError(QString error) {
    Q_UNUSED(error);
    // TODO: emit the signal
}

void
SMFileDownload::emitNetworkError(QNetworkReply::NetworkError code) {
    Q_UNUSED(code);
    // TODO: emit the signal
}

void
SMFileDownload::emitSslError(const QList<QSslError>& errors) {
    Q_UNUSED(errors);
    // TODO: emit the signal
}

}  // Ubuntu

}  // DownloadManager
