/*
 * Copyright 2013-2015 Canonical Ltd.
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

#include <QObject>
#include <ubuntu/download_manager/metatypes.h>
#include <ubuntu/downloads/manager.h>  // comes from the priv lib, just for testing!!!!

using namespace Ubuntu::Transfers::System;
using namespace Ubuntu::DownloadManager::Daemon;

class TestingManager : public DownloadManager {
    Q_OBJECT

 public:
    TestingManager(Application* app,
            DBusConnection* connection,
            bool stoppable = false,
            QObject *parent = 0)
        : DownloadManager(app, connection, stoppable, parent) {}

 public slots:  // NOLINT(whitespace/indent)
    QDBusObjectPath createDownload(DownloadStruct download) override;

    QDBusObjectPath createDownloadGroup(StructList downloads,
                                        const QString& algorithm,
                                        bool allowed3G,
                                        const QVariantMap& metadata,
                                        StringMap headers) override;

    qulonglong defaultThrottle() override;
    void setDefaultThrottle(qulonglong speed) override;
    void allowGSMDownload(bool allowed) override;
    bool isGSMDownloadAllowed() override;
    QList<QDBusObjectPath> getAllDownloads(const QString& appId,
                                           bool uncollected) override;
    QList<QDBusObjectPath> getAllDownloadsWithMetadata(
                                              const QString& name,
                                              const QString& value) override;
    void exit() override;

    void returnDBusErrors(bool errors);
    void returnHttpError(const QString &download, HttpErrorStruct error);
    void returnNetworkError(const QString &download, NetworkErrorStruct error);
    void returnProcessError(const QString &download, ProcessErrorStruct error);
    void returnAuthError(const QString &download, AuthErrorStruct error);
    void returnHashError(const QString &download, HashErrorStruct error);

 protected:
    QDBusObjectPath registerDownload(Download* download) override;
 private:
    bool _returnErrors = false;
};

