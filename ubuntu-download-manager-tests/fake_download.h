/*
 * Copyright 2013 2013 Canonical Ltd.
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
#include <download.h>
#include <system_network_info.h>
#include <metatypes.h>
#include "fake.h"

class FakeDownload : public Download, public Fake
{
    Q_OBJECT
public:
    explicit FakeDownload(const QUuid& id, const QString& path, const QUrl& url, const QVariantMap& metadata,
        const QMap<QString, QString>& headers, SystemNetworkInfo* networkInfo, RequestFactory* nam, QObject* parent=0);
    explicit FakeDownload(const QUuid& id, const QString& path, const QUrl& url, const QString& hash,
        QCryptographicHash::Algorithm algo, const QVariantMap& metadata, const QMap<QString, QString> &headers,
        SystemNetworkInfo* networkInfo, RequestFactory* nam, QObject* parent=0);

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
    
private:
    bool _canDownload;
};

#endif // FAKE_DOWNLOAD_H
