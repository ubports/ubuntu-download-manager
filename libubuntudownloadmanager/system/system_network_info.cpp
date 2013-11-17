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

#include <QDebug>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#include "logger.h"
#include "system_network_info.h"

namespace Ubuntu {

namespace DownloadManager {

namespace System {

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
        _configMan = new QNetworkConfigurationManager();

#ifdef DEBUG
        // in debug do log the changes else just fwd them
        q->connect(_info, SIGNAL(cellIdChanged(int, const QString&)), q,
            SLOT(onCellIdChanged(int, const QString&)));
        q->connect(_info, SIGNAL(currentCellDataTechnologyChanged(int, QNetworkInfo::CellDataTechnology)), q,
            SLOT(onCurrentCellDataTechnologyChanged(int, QNetworkInfo::CellDataTechnology)));
        q->connect(_info, SIGNAL(currentMobileCountryCodeChanged(int, const QString&)), q,
            SLOT(onCurrentMobileCountryCodeChanged(int, const QString&)));
        q->connect(_info, SIGNAL(currentMobileNetworkCodeChanged(int, const QString&)), q,
            SLOT(onCurrentMobileNetworkCodeChanged(int, const QString&)));
        q->connect(_info, SIGNAL(currentNetworkModeChanged(QNetworkInfo::NetworkMode)), q,
            SLOT(onCurrentNetworkModeChanged(QNetworkInfo::NetworkMode)));
        q->connect(_info, SIGNAL(locationAreaCodeChanged(int, const QString&)), q,
            SLOT(onLocationAreaCodeChanged(int, const QString&)));
        q->connect(_info, SIGNAL(networkInterfaceCountChanged(QNetworkInfo::NetworkMode, int)), q,
            SLOT(onNetworkInterfaceCountChanged(QNetworkInfo::NetworkMode, int)));
        q->connect(_info, SIGNAL(networkNameChanged(QNetworkInfo::NetworkMode, int, const QString&)), q,
            SLOT(onNetworkNameChanged(QNetworkInfo::NetworkMode, int, const QString&)));
        q->connect(_info, SIGNAL(networkSignalStrengthChanged(QNetworkInfo::NetworkMode, int, int)), q,
            SLOT(onNetworkSignalStrengthChanged(QNetworkInfo::NetworkMode, int, int)));
        q->connect(_info, SIGNAL(networkStatusChanged(QNetworkInfo::NetworkMode, int, QNetworkInfo::NetworkStatus)), q,
            SLOT(onNetworkStatusChanged(QNetworkInfo::NetworkMode, int, QNetworkInfo::NetworkStatus)));

        q->connect(_configMan,
            SIGNAL(onlineStateChanged(bool)), q,
            SLOT(onOnlineStateChanged(bool)));
#else
        // connect to interesting signals
        q->connect(_info, &QNetworkInfo::cellIdChanged, q,
            &SystemNetworkInfo::cellIdChanged);
        q->connect(_info, &QNetworkInfo::currentCellDataTechnologyChanged, q,
            &SystemNetworkInfo::currentCellDataTechnologyChanged);
        q->connect(_info, &QNetworkInfo::currentMobileCountryCodeChanged, q,
            &SystemNetworkInfo::currentMobileCountryCodeChanged);
        q->connect(_info, &QNetworkInfo::currentMobileNetworkCodeChanged, q,
            &SystemNetworkInfo::currentMobileNetworkCodeChanged);
        q->connect(_info, &QNetworkInfo::currentNetworkModeChanged, q,
            &SystemNetworkInfo::currentNetworkModeChanged);
        q->connect(_info, &QNetworkInfo::locationAreaCodeChanged, q,
            &SystemNetworkInfo::locationAreaCodeChanged);
        q->connect(_info, &QNetworkInfo::networkInterfaceCountChanged, q,
            &SystemNetworkInfo::networkInterfaceCountChanged);
        q->connect(_info, &QNetworkInfo::networkNameChanged, q,
            &SystemNetworkInfo::networkNameChanged);
        q->connect(_info, &QNetworkInfo::networkSignalStrengthChanged, q,
            &SystemNetworkInfo::networkSignalStrengthChanged);
        q->connect(_info, &QNetworkInfo::networkStatusChanged, q,
            &SystemNetworkInfo::networkStatusChanged);

        q->connect(_configMan,
            &QNetworkConfigurationManager::onlineStateChanged, q,
            &SystemNetworkInfo::onlineStateChanged);
#endif

    }

    ~SystemNetworkInfoPrivate() {
        delete _info;
        delete _configMan;
    }

    QNetworkInfo::NetworkMode currentNetworkMode() {
        return _info->currentNetworkMode();
    }

    bool isOnline() {
        return _configMan->isOnline();
    }

    void onOnlineStateChanged(bool online) {
        Q_Q(SystemNetworkInfo);
        TRACE << online;
        emit q->onlineStateChanged(online);
    }

#if DEBUG

    void onCellIdChanged(int interface, const QString& id) {
        Q_Q(SystemNetworkInfo);
        TRACE << interface << id;
        emit q->cellIdChanged(interface, id);
    }

    void onCurrentCellDataTechnologyChanged(int interface,
                                    QNetworkInfo::CellDataTechnology tech) {
        Q_Q(SystemNetworkInfo);
        TRACE << interface << tech;
        emit q->currentCellDataTechnologyChanged(interface, tech);
    }

    void onCurrentMobileCountryCodeChanged(int interface, const QString& mcc) {
        Q_Q(SystemNetworkInfo);
        TRACE << interface << mcc;
        emit q->currentMobileCountryCodeChanged(interface, mcc);
    }

    void onCurrentMobileNetworkCodeChanged(int interface, const QString& mnc) {
        Q_Q(SystemNetworkInfo);
        TRACE << interface << mnc;
        emit q->currentMobileNetworkCodeChanged(interface, mnc);
    }

    void onCurrentNetworkModeChanged(QNetworkInfo::NetworkMode mode) {
        Q_Q(SystemNetworkInfo);
        TRACE << mode;
        emit q->currentNetworkModeChanged(mode);
    }

    void onLocationAreaCodeChanged(int interface, const QString& lac) {
        Q_Q(SystemNetworkInfo);
        TRACE << interface << lac;
        emit q->locationAreaCodeChanged(interface, lac);
    }

    void onNetworkInterfaceCountChanged(QNetworkInfo::NetworkMode mode,
                                        int count) {
        Q_Q(SystemNetworkInfo);
        TRACE << mode << count;
        emit q->networkInterfaceCountChanged(mode, count);
    }

    void onNetworkNameChanged(QNetworkInfo::NetworkMode mode, int interface,
                              const QString& name) {
        Q_Q(SystemNetworkInfo);
        TRACE << mode << interface << name;
        emit q->networkNameChanged(mode, interface, name);
    }

    void onNetworkSignalStrengthChanged(QNetworkInfo::NetworkMode mode,
                                        int interface, int strength) {
        Q_Q(SystemNetworkInfo);
        TRACE << mode << interface << strength;
        emit q->networkSignalStrengthChanged(mode, interface, strength);
    }

    void onNetworkStatusChanged(QNetworkInfo::NetworkMode mode, int interface,
                                QNetworkInfo::NetworkStatus status) {
        Q_Q(SystemNetworkInfo);
        TRACE << mode << interface << status;
        emit q->networkStatusChanged(mode, interface, status);
    }

#endif

 private:
    QNetworkInfo* _info;
    QNetworkConfigurationManager* _configMan;
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

bool
SystemNetworkInfo::isOnline() {
    Q_D(SystemNetworkInfo);
    return d->isOnline();
}

}  // System

}  // DownloadManager

}  // Ubuntu

#include "moc_system_network_info.cpp"
