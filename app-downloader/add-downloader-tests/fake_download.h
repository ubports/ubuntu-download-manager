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
#include <app_download.h>
#include "fake.h"

class FakeDownload : public AppDownload, public Fake
{
    Q_OBJECT
public:
    explicit FakeDownload(QString appId, QString appName, QString path, QUrl url, RequestFactory* nam, QObject* parent=0);
    explicit FakeDownload(QString appId, QString appName, QString path, QUrl url, QString hash, QCryptographicHash::Algorithm algo,
        RequestFactory* nam, QObject* parent=0);

    void cancelDownload() override;
    void pauseDownload() override;
    void resumeDownload() override;
    void startDownload() override;
    
};

#endif // FAKE_DOWNLOAD_H
