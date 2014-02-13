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

#ifndef FAKE_GROUP_DOWNLOAD_H
#define FAKE_GROUP_DOWNLOAD_H

#include <downloads/group_download.h>
#include "download.h"

class FakeGroupDownload : public GroupDownload {
    Q_OBJECT

 public:
    FakeGroupDownload(const QString& id,
                      const QString& path,
                      bool isConfined,
                      const QString& rootPath,
                      QList<GroupDownloadStruct> downloads,
                      const QString& algo,
                      bool isGSMDownloadAllowed,
                      const QVariantMap& metadata,
                      const QMap<QString, QString>& headers,
                      Factory* downFactory,
                      QObject* parent = 0);

    void emitAuthError(const QString& url, AuthErrorStruct error);
    void emitHttpError(const QString& url, HttpErrorStruct error);
    void emitNetworkError(const QString& url, NetworkErrorStruct error);
    void emitProcessError(const QString& url, ProcessErrorStruct error);

 private:
    FakeDownload* downloadForUrl(const QString& url);
};

#endif // GROUP_DOWNLOAD_H
