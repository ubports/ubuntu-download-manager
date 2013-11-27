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

SystemNetworkInfo* SystemNetworkInfo::_instance = NULL;
QMutex SystemNetworkInfo::_mutex;

SystemNetworkInfo::SystemNetworkInfo(QObject* parent)
    : QObject(parent) {
    _info = new QNetworkInfo(this);
    _configMan = new QNetworkConfigurationManager(this);

#ifdef DEBUG
    // in debug do log the changes else just fwd them
    connect(_info, &QNetworkInfo::cellIdChanged, this,
        &SystemNetworkInfo::onCellIdChanged);
    connect(_info, &QNetworkInfo::currentCellDataTechnologyChanged, this,
        &SystemNetworkInfo::onCurrentCellDataTechnologyChanged);
    connect(_info, &QNetworkInfo::currentMobileCountryCodeChanged, this,
        &SystemNetworkInfo::onCurrentMobileCountryCodeChanged);
    connect(_info, &QNetworkInfo::currentMobileNetworkCodeChanged, this,
        &SystemNetworkInfo::onCurrentMobileNetworkCodeChanged);
    connect(_info, &QNetworkInfo::currentNetworkModeChanged, this,
        &SystemNetworkInfo::onCurrentNetworkModeChanged);
    connect(_info, &QNetworkInfo::locationAreaCodeChanged, this,
        &SystemNetworkInfo::onLocationAreaCodeChanged);
    connect(_info, &QNetworkInfo::networkInterfaceCountChanged, this,
        &SystemNetworkInfo::onNetworkInterfaceCountChanged);
    connect(_info, &QNetworkInfo::networkNameChanged, this,
        &SystemNetworkInfo::onNetworkNameChanged);
    connect(_info, &QNetworkInfo::networkSignalStrengthChanged, this,
        &SystemNetworkInfo::onNetworkSignalStrengthChanged);
    connect(_info, &QNetworkInfo::networkStatusChanged, this,
        &SystemNetworkInfo::onNetworkStatusChanged);

    connect(_configMan,
        &QNetworkConfigurationManager::onlineStateChanged, this,
        &SystemNetworkInfo::onOnlineStateChanged);
#else
    // connect to interesting signals
    connect(_info, &QNetworkInfo::cellIdChanged, q,
        &SystemNetworkInfo::cellIdChanged);
    connect(_info, &QNetworkInfo::currentCellDataTechnologyChanged, q,
        &SystemNetworkInfo::currentCellDataTechnologyChanged);
    connect(_info, &QNetworkInfo::currentMobileCountryCodeChanged, q,
        &SystemNetworkInfo::currentMobileCountryCodeChanged);
    connect(_info, &QNetworkInfo::currentMobileNetworkCodeChanged, q,
        &SystemNetworkInfo::currentMobileNetworkCodeChanged);
    connect(_info, &QNetworkInfo::currentNetworkModeChanged, q,
        &SystemNetworkInfo::currentNetworkModeChanged);
    connect(_info, &QNetworkInfo::locationAreaCodeChanged, q,
        &SystemNetworkInfo::locationAreaCodeChanged);
    connect(_info, &QNetworkInfo::networkInterfaceCountChanged, q,
        &SystemNetworkInfo::networkInterfaceCountChanged);
    connect(_info, &QNetworkInfo::networkNameChanged, q,
        &SystemNetworkInfo::networkNameChanged);
    connect(_info, &QNetworkInfo::networkSignalStrengthChanged, q,
        &SystemNetworkInfo::networkSignalStrengthChanged);
    connect(_info, &QNetworkInfo::networkStatusChanged, q,
        &SystemNetworkInfo::networkStatusChanged);

    connect(_configMan,
        &QNetworkConfigurationManager::onlineStateChanged, q,
        &SystemNetworkInfo::onlineStateChanged);
#endif

}

QNetworkInfo::NetworkMode
SystemNetworkInfo::currentNetworkMode() {
    return _info->currentNetworkMode();
}

bool
SystemNetworkInfo::isOnline() {
    return _configMan->isOnline();
}

SystemNetworkInfo*
SystemNetworkInfo::instance() {
    if(_instance == NULL) {
        _mutex.lock();
        if(_instance == NULL)
            _instance = new SystemNetworkInfo();
        _mutex.unlock();
    }
    return _instance;
}

void
SystemNetworkInfo::setInstance(SystemNetworkInfo* instance) {
    _instance = instance;
}

void
SystemNetworkInfo::deleteInstance() {
    if(_instance != NULL) {
        _mutex.lock();
        if(_instance != NULL) {
            delete _instance;
            _instance = NULL;
        }
        _mutex.unlock();
    }
}

void
SystemNetworkInfo::onOnlineStateChanged(bool online) {
    TRACE << online;
    emit onlineStateChanged(online);
}

#if DEBUG

void
SystemNetworkInfo::onCellIdChanged(int interface, const QString& id) {
    TRACE << interface << id;
    emit cellIdChanged(interface, id);
}

void
SystemNetworkInfo::onCurrentCellDataTechnologyChanged(int interface,
                                QNetworkInfo::CellDataTechnology tech) {
    TRACE << interface << tech;
    emit currentCellDataTechnologyChanged(interface, tech);
}

void
SystemNetworkInfo::onCurrentMobileCountryCodeChanged(int interface, const QString& mcc) {
    TRACE << interface << mcc;
    emit currentMobileCountryCodeChanged(interface, mcc);
}

void
SystemNetworkInfo::onCurrentMobileNetworkCodeChanged(int interface, const QString& mnc) {
    TRACE << interface << mnc;
    emit currentMobileNetworkCodeChanged(interface, mnc);
}

void
SystemNetworkInfo::onCurrentNetworkModeChanged(QNetworkInfo::NetworkMode mode) {
    TRACE << mode;
    emit currentNetworkModeChanged(mode);
}

void
SystemNetworkInfo::onLocationAreaCodeChanged(int interface, const QString& lac) {
    TRACE << interface << lac;
    emit locationAreaCodeChanged(interface, lac);
}

void
SystemNetworkInfo::onNetworkInterfaceCountChanged(QNetworkInfo::NetworkMode mode,
                                                  int count) {
    TRACE << mode << count;
    emit networkInterfaceCountChanged(mode, count);
}

void
SystemNetworkInfo::onNetworkNameChanged(QNetworkInfo::NetworkMode mode, int interface,
                                        const QString& name) {
    TRACE << mode << interface << name;
    emit networkNameChanged(mode, interface, name);
}

void
SystemNetworkInfo::onNetworkSignalStrengthChanged(QNetworkInfo::NetworkMode mode,
                                                  int interface, int strength) {
    TRACE << mode << interface << strength;
    emit networkSignalStrengthChanged(mode, interface, strength);
}

void
SystemNetworkInfo::onNetworkStatusChanged(QNetworkInfo::NetworkMode mode, int interface,
                                          QNetworkInfo::NetworkStatus status) {
    TRACE << mode << interface << status;
    emit networkStatusChanged(mode, interface, status);
}

#endif

}  // System

}  // DownloadManager

}  // Ubuntu
