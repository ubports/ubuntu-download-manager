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

#ifndef FAKE_DOWNLOAD_H
#define FAKE_DOWNLOAD_H

#include <QSharedPointer>
#include <ubuntu/downloads/file_download.h>
#include <gmock/gmock.h>

using namespace Ubuntu::DownloadManager::Daemon;

class MockDownload : public FileDownload {
 public:
    MockDownload(const QString& id,
                 const QString& path,
                 bool isConfined,
                 const QString& rootPath,
                 const QUrl& url,
                 const QVariantMap& metadata,
                 const QMap<QString, QString>& headers,
                 QObject* parent = 0)
        : FileDownload(id, "", path, isConfined, rootPath,
            url, metadata, headers, parent) {}

    MockDownload(const QString& id,
                 const QString& appId,
                 const QString& path,
                 bool isConfined,
                 const QString& rootPath,
                 const QUrl& url,
                 const QVariantMap& metadata,
                 const QMap<QString, QString>& headers,
                 QObject* parent = 0)
        : FileDownload(id, appId, path, isConfined, rootPath,
            url, metadata, headers, parent) {}

    MockDownload(const QString& id,
                 const QString& appId,
                 const QString& path,
                 bool isConfined,
                 const QString& rootPath,
                 const QUrl& url,
                 const QString& hash,
                 const QString& algo,
                 const QVariantMap& metadata,
                 const QMap<QString, QString>& headers,
                 QObject* parent = 0)
        : FileDownload(id, appId, path, isConfined, rootPath, url, hash,
            algo, metadata, headers, parent) {}

    MockDownload(const QString& id,
                 const QString& path,
                 bool isConfined,
                 const QString& rootPath,
                 const QUrl& url,
                 const QString& hash,
                 const QString& algo,
                 const QVariantMap& metadata,
                 const QMap<QString, QString>& headers,
                 QObject* parent = 0)
        : FileDownload(id, "", path, isConfined, rootPath, url, hash,
            algo, metadata, headers, parent) {}

    MOCK_METHOD0(canDownload, bool());
    MOCK_METHOD1(setThrottle, void(qulonglong));
    MOCK_METHOD0(throttle, qulonglong());
    MOCK_METHOD0(cancelTransfer, void());
    MOCK_METHOD0(pauseTransfer, void());
    MOCK_METHOD0(resumeTransfer, void());
    MOCK_METHOD0(startTransfer, void());
    MOCK_CONST_METHOD0(isValid, bool());
    MOCK_CONST_METHOD0(filePath, QString());
    MOCK_CONST_METHOD0(state, Transfer::State());
    MOCK_CONST_METHOD0(path, QString());
    MOCK_CONST_METHOD0(metadata, QVariantMap());
    MOCK_METHOD1(allowGSMDownload, void(bool));
    MOCK_METHOD0(isGSMDownloadAllowed, bool());

    // useful methods to emit signals
    using FileDownload::finished;
    using FileDownload::authError;
    using FileDownload::httpError;
    using FileDownload::networkError;
    using FileDownload::processError;

    using Download::setAddToQueue;

    using Download::error;
};

#endif  // FAKE_DOWNLOAD_H
