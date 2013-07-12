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

#include "fake_system_network_info.h"

NetworkModeWrapper::NetworkModeWrapper(QNetworkInfo::NetworkMode mode, QObject *parent):
    QObject(parent)
{
    _mode = mode;
}

QNetworkInfo::NetworkMode NetworkModeWrapper::mode()
{
    return _mode;
}

void NetworkModeWrapper::setMode(QNetworkInfo::NetworkMode mode)
{
    _mode = mode;
}

FakeSystemNetworkInfo::FakeSystemNetworkInfo(QObject *parent) :
    SystemNetworkInfo(parent)
{
}

QNetworkInfo::NetworkMode FakeSystemNetworkInfo::currentNetworkMode()
{
    if (_recording)
    {
        QList<QObject*> inParams;

        QList<QObject*> outParams;
        outParams.append(new NetworkModeWrapper(_mode, this));

        MethodParams params(inParams, outParams);
        MethodData methodData("currentNetworkMode", params);
        _called.append(methodData);
    }
    return _mode;
}

QNetworkInfo::NetworkMode FakeSystemNetworkInfo::mode()
{
    return _mode;
}

void FakeSystemNetworkInfo::setMode(QNetworkInfo::NetworkMode mode)
{
    _mode = mode;
}
