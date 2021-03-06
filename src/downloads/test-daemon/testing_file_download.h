/*
 * Copyright 2014-2015 Canonical Ltd.
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

#pragma once

#include <QDBusContext>
#include <QObject>
#include <ubuntu/downloads/file_download.h>
#include <ubuntu/download_manager/metatypes.h>

using namespace Ubuntu::DownloadManager::Daemon;

class TestingFileDownload : public FileDownload {
    Q_OBJECT

 public:
    TestingFileDownload(FileDownload* down,
                        QObject* parent = 0);
    virtual ~TestingFileDownload();
    void returnDBusErrors(bool errors);
    void returnHttpError(HttpErrorStruct error);
    void returnNetworkError(NetworkErrorStruct error);
    void returnProcessError(ProcessErrorStruct error);
    void returnAuthError(AuthErrorStruct error);
    void returnHashError(HashErrorStruct error);

 public slots:  // NOLINT(whitespace/indent)
    qulonglong progress() override;
    qulonglong totalSize() override;

    void setThrottle(qulonglong speed) override;
    qulonglong throttle();

    void setHeaders(StringMap headers) override;
    StringMap headers();

    void allowGSMDownload(bool allowed) override;
    bool isGSMDownloadAllowed() override;

    QVariantMap metadata() const override;
    void setMetadata(QVariantMap metadata) const;

    Ubuntu::DownloadManager::Daemon::Download::State state() const override;

    void cancel() override;
    void pause() override;
    void resume() override;
    void start() override;

    void cancelTransfer() override;
    void pauseTransfer() override;
    void resumeTransfer() override;
    void startTransfer() override;

 private:
    bool _returnErrors = false;
    bool _returnAuthError = false;
    bool _returnHttpError = false;
    bool _returnNetworkError = false;
    bool _returnProcessError = false;
    bool _returnHashError = false;
    AuthErrorStruct _authErr;
    HttpErrorStruct _httpErr;
    NetworkErrorStruct _networkErr;
    ProcessErrorStruct _processErr;
    HashErrorStruct _hashErr;
    FileDownload* _down = nullptr;
};

