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

#include "group_download.h"

FakeGroupDownload::FakeGroupDownload(const QString& id,
                                     const QString& path,
                                     bool isConfined,
                                     const QString& rootPath,
                                     QList<GroupDownloadStruct> downloads,
                                     const QString& algo,
                                     bool isGSMDownloadAllowed,
                                     const QVariantMap& metadata,
                                     const QMap<QString, QString>& headers,
                                     Factory* downFactory,
                                     QObject* parent)
    : GroupDownload(id, path, isConfined, rootPath, downloads, algo,
           isGSMDownloadAllowed, metadata, headers, downFactory, parent) {
}

FakeDownload*
FakeGroupDownload::downloadForUrl(const QString& url) {
    // loop in the downloads and return a fake download if possible
    // null otherwise
    foreach(FileDownload* down, _downloads) {
        if (down->url() == url) {
            auto result = qobject_cast<FakeDownload*>(down);
            return result;
        }
    }
    return nullptr;
}

void
FakeGroupDownload::emitAuthError(const QString& url, AuthErrorStruct error) {
    auto down = downloadForUrl(url);
    if (down != nullptr) {
        down->emitAuthError(error);
    }
}

void
FakeGroupDownload::emitHttpError(const QString& url, HttpErrorStruct error) {
    auto down = downloadForUrl(url);
    if (down != nullptr) {
        down->emitHttpError(error);
    }
}

void
FakeGroupDownload::emitNetworkError(const QString& url,
                                    NetworkErrorStruct error) {
    auto down = downloadForUrl(url);
    if (down != nullptr) {
        down->emitNetworkError(error);
    }
}

void
FakeGroupDownload::emitProcessError(const QString& url,
                                    ProcessErrorStruct error) {
    auto down = downloadForUrl(url);
    if (down != nullptr) {
        down->emitProcessError(error);
    }
}
