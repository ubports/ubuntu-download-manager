/*
 * Copyright 2013 Canonical Ltd.
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

#include "fake_qnetwork_reply.h"

FakeQNetworkReply::FakeQNetworkReply(QObject* parent) :
    QNetworkReply(parent),
    Fake()
{
}

void FakeQNetworkReply::abort()
{
    if (_recording)
    {
        MethodData methodData;
        methodData.setMethodName("abort");
        _called.append(methodData);
    }
}

void FakeQNetworkReply::deleteLater()
{
    if (_recording)
    {
        MethodData methodData;
        methodData.setMethodName("deleteLater");
        _called.append(methodData);
    }

    QObject::deleteLater();
}

qint64 FakeQNetworkReply::bytesAvailable() const
{
    return 0;
}
qint64 FakeQNetworkReply::bytesToWrite() const
{
    return -1;
}

bool FakeQNetworkReply::canReadLine() const {
    qFatal("not implemented"); return false;
}

bool FakeQNetworkReply::waitForReadyRead(int) {
    qFatal("not implemented"); return false;
}

bool FakeQNetworkReply::waitForBytesWritten(int) {
    qFatal("not implemented"); return false;
}

qint64 FakeQNetworkReply::readData(char* data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)
    return 0;
}
