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

FakeDownload::FakeDownload(QString appId, QString appName, QString path, QUrl url, RequestFactory* nam, QObject* parent):
    Download(appId, appName, path, url, nam, parent)
{
}

FakeDownload::FakeDownload(QString appId, QString appName, QString path, QUrl url, QString hash, QCryptographicHash::Algorithm algo,
    RequestFactory* nam, QObject* parent):
        Download(appId, appName, path, url, hash, algo, nam, parent)
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

