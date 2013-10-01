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

#include "./system_network_info.h"


/*
 * PRIVATE IMPLEMENTATION
 */

class SystemNetworkInfoPrivate {
    Q_DECLARE_PUBLIC(SystemNetworkInfo)

 public:
    explicit SystemNetworkInfoPrivate(SystemNetworkInfo* parent)
        : q_ptr(parent) {
        Q_Q(SystemNetworkInfo);
        _info = new QNetworkInfo();
        _man = new QNetworkAccessManager();

        // connect to interesting signals
        q->connect(_info,
            SIGNAL(currentNetworkModeChanged(QNetworkInfo::NetworkMode)), q,
            SIGNAL(currentNetworkModeChanged(QNetworkInfo::NetworkMode)));
        q->connect(_info,
            SIGNAL(networkStatusChanged(QNetworkInfo::NetworkMode, int, QNetworkInfo::NetworkStatus)), q,
            SIGNAL(networkStatusChanged(QNetworkInfo::NetworkMode, int, QNetworkInfo::NetworkStatus)));
        q->connect(_man,
            SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)), q,
            SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)));
    }

    ~SystemNetworkInfoPrivate() {
        if (_info != NULL)
            delete _info;
        if (_man != NULL)
            delete _man;
    }

    QNetworkInfo::NetworkMode currentNetworkMode() {
        return _info->currentNetworkMode();
    }

 private:
    QNetworkAccessManager* _man;
    QNetworkInfo* _info;
    SystemNetworkInfo* q_ptr;
};


/*
 * PUBLIC IMPLEMENTATION
 */

SystemNetworkInfo::SystemNetworkInfo(QObject *parent)
    : QObject(parent),
      d_ptr(new SystemNetworkInfoPrivate(this)) {
}

QNetworkInfo::NetworkMode
SystemNetworkInfo::currentNetworkMode() {
    Q_D(SystemNetworkInfo);
    return d->currentNetworkMode();
}
