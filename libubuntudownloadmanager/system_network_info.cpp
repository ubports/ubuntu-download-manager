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

#include "system_network_info.h"


/*
 * PRIVATE IMPLEMENTATION
 */

class SystemNetworkInfoPrivate
{
    Q_DECLARE_PUBLIC(SystemNetworkInfo)
public:
    explicit SystemNetworkInfoPrivate(SystemNetworkInfo* parent);
    ~SystemNetworkInfoPrivate();

    QNetworkInfo::NetworkMode currentNetworkMode();
    void onCurrentNetworkModeChanged(QNetworkInfo::NetworkMode mode);

private:
    QNetworkInfo* _info;
    SystemNetworkInfo* q_ptr;

};

SystemNetworkInfoPrivate::SystemNetworkInfoPrivate(SystemNetworkInfo* parent):
    q_ptr(parent)
{
    Q_Q(SystemNetworkInfo);
    _info = new QNetworkInfo();

    // connect to interesting signal
    q->connect(_info, SIGNAL(currentNetworkModeChanged(QNetworkInfo::NetworkMode)),
        q, SLOT(onCurrentNetworkModeChanged(QNetworkInfo::NetworkMode)));
}

SystemNetworkInfoPrivate::~SystemNetworkInfoPrivate()
{
    if (_info != NULL)
        delete _info;
}

QNetworkInfo::NetworkMode SystemNetworkInfoPrivate::currentNetworkMode()
{
    return _info->currentNetworkMode();
}

void SystemNetworkInfoPrivate::onCurrentNetworkModeChanged(QNetworkInfo::NetworkMode mode)
{
    // foward the signal
    Q_Q(SystemNetworkInfo);
    emit q->currentNetworkModeChanged(mode);
}

/*
 * PUBLIC IMPLEMNTATION
 */

SystemNetworkInfo::SystemNetworkInfo(QObject *parent) :
    QObject(parent),
    d_ptr(new SystemNetworkInfoPrivate(this))
{
}

QNetworkInfo::NetworkMode SystemNetworkInfo::currentNetworkMode()
{
    Q_D(SystemNetworkInfo);
    return d->currentNetworkMode();
}

#include "moc_system_network_info.cpp"
