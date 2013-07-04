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

#include "fake_download.h"

FakeDownload::FakeDownload(const QUuid& id, const QString& path, const QUrl& url, const QVariantMap& metadata,
    const QVariantMap& headers, RequestFactory* nam, QObject* parent):
        Download(id, path, url, metadata, headers, nam, parent)
{
}

FakeDownload::FakeDownload(const QUuid& id, const QString& path, const QUrl& url, const QString& hash, QCryptographicHash::Algorithm algo,
        const QVariantMap& metadata, const QVariantMap& headers, RequestFactory* nam, QObject* parent) :
        Download(id, path, url, hash, algo, metadata, headers, nam, parent)
{
}

void FakeDownload::cancelDownload()
{
    if (_recording)
    {
        MethodData methodData;
        methodData.setMethodName("cancelDownload");
        _called.append(methodData);
    }
}

void FakeDownload::pauseDownload()
{
    if (_recording)
    {
        MethodData methodData;
        methodData.setMethodName("pauseDownload");
        _called.append(methodData);
    }
}

void FakeDownload::resumeDownload()
{
    if (_recording)
    {
        MethodData methodData;
        methodData.setMethodName("resumeDownload");
        _called.append(methodData);
    }
}

void FakeDownload::startDownload()
{
    if (_recording)
    {
        MethodData methodData;
        methodData.setMethodName("startDownload");
        _called.append(methodData);
    }
}

