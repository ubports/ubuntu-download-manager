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
#include <ubuntu/transfers/system/system_network_info.h>
#include <ubuntu/transfers/tests/fake.h>
#include <ubuntu/download_manager/metatypes.h>

using namespace Ubuntu::DownloadManager::Daemon;
using namespace Ubuntu::Transfers::Tests;

class FakeDownload : public FileDownload, public Fake {
    Q_OBJECT

 public:
    FakeDownload(const QString& id,
                 const QString& path,
                 bool isConfined,
                 const QString& rootPath,
                 const QUrl& url,
                 const QVariantMap& metadata,
                 const QMap<QString, QString>& headers,
                 QObject* parent = 0);
    FakeDownload(const QString& id,
                 const QString& path,
                 bool isConfined,
                 const QString& rootPath,
                 const QUrl& url,
                 const QString& hash,
                 const QString& algo,
                 const QVariantMap& metadata,
                 const QMap<QString, QString> &headers,
                 QObject* parent = 0);

    bool canDownload() override;
    void setCanDownload(bool canDownload);
    void setThrottle(qulonglong speed) override;
    qulonglong throttle() override;
    void cancelDownload() override;
    void pauseDownload() override;
    void resumeDownload() override;
    void startDownload() override;

    // useful methods to emit signals
    void emitFinished(const QString& path);
    void emitError(const QString& error);
    void emitAuthError(AuthErrorStruct error);
    void emitHttpError(HttpErrorStruct error);
    void emitNetworkError(NetworkErrorStruct error);
    void emitProcessError(ProcessErrorStruct error);

    using Download::setAddToQueue;

 private:
    bool _canDownload;
};

#endif  // FAKE_DOWNLOAD_H
