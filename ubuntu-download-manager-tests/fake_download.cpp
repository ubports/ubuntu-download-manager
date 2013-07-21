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
    const QMap<QString, QString> &headers, SystemNetworkInfo* networkInfo, RequestFactory* nam, ProcessFactory* processFactory, QObject* parent):
        Download(id, path, url, metadata, headers, networkInfo, nam, processFactory, parent),
        _canDownload(true)
{
}

FakeDownload::FakeDownload(const QUuid& id, const QString& path, const QUrl& url, const QString& hash, QCryptographicHash::Algorithm algo,
        const QVariantMap& metadata, const QMap<QString ,QString>& headers, SystemNetworkInfo* networkInfo, RequestFactory* nam,
        ProcessFactory* processFactory, QObject* parent) :
        Download(id, path, url, hash, algo, metadata, headers, networkInfo, nam, processFactory, parent),
        _canDownload(true)
{
}

bool FakeDownload::canDownload()
{
    if (_recording)
    {
        MethodData methodData;
        methodData.setMethodName("canDownload");
        _called.append(methodData);
    }
    return _canDownload;
}

void FakeDownload::setCanDownload(bool canDownload)
{
    _canDownload = canDownload;
}

void FakeDownload::setThrottle(qlonglong speed)
{
    if (_recording)
    {
        QList<QObject*> inParams;
        inParams.append(new UintWrapper(speed));

        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("setThrottle", params);
        _called.append(methodData);
    }
   Download::setThrottle(speed);
}

qlonglong FakeDownload::throttle()
{
    if (_recording)
    {
        MethodData methodData;
        methodData.setMethodName("throttle");
        _called.append(methodData);
    }
    return Download::throttle();
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

