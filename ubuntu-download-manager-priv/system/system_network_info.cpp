/*
 * Copyright 2013-2014 Canonical Ltd.
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
    connect(_info, &QNetworkInfo::cellIdChanged, this,
        &SystemNetworkInfo::cellIdChanged);
    connect(_info, &QNetworkInfo::currentCellDataTechnologyChanged, this,
        &SystemNetworkInfo::currentCellDataTechnologyChanged);
    connect(_info, &QNetworkInfo::currentMobileCountryCodeChanged, this,
        &SystemNetworkInfo::currentMobileCountryCodeChanged);
    connect(_info, &QNetworkInfo::currentMobileNetworkCodeChanged, this,
        &SystemNetworkInfo::currentMobileNetworkCodeChanged);
    connect(_info, &QNetworkInfo::currentNetworkModeChanged, this,
        &SystemNetworkInfo::currentNetworkModeChanged);
    connect(_info, &QNetworkInfo::locationAreaCodeChanged, this,
        &SystemNetworkInfo::locationAreaCodeChanged);
    connect(_info, &QNetworkInfo::networkInterfaceCountChanged, this,
        &SystemNetworkInfo::networkInterfaceCountChanged);
    connect(_info, &QNetworkInfo::networkNameChanged, this,
        &SystemNetworkInfo::networkNameChanged);
    connect(_info, &QNetworkInfo::networkSignalStrengthChanged, this,
        &SystemNetworkInfo::networkSignalStrengthChanged);
    connect(_info, &QNetworkInfo::networkStatusChanged, this,
        &SystemNetworkInfo::networkStatusChanged);

    connect(_configMan,
        &QNetworkConfigurationManager::onlineStateChanged, this,
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
        LOG(INFO) << "Instance is null";
        _mutex.lock();
        if(_instance == NULL){
            LOG(INFO) << "Create new instance";
            _instance = new SystemNetworkInfo();
        }
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
    DLOG(INFO) << " " << __PRETTY_FUNCTION__ << online;
    emit onlineStateChanged(online);
}

#if DEBUG

void
SystemNetworkInfo::onCellIdChanged(int interface, const QString& id) {
    DLOG(INFO) << " " << __PRETTY_FUNCTION__ << interface << id;
    emit cellIdChanged(interface, id);
}

void
SystemNetworkInfo::onCurrentCellDataTechnologyChanged(int interface,
                                QNetworkInfo::CellDataTechnology tech) {
    DLOG(INFO) << " " << __PRETTY_FUNCTION__ << interface << tech;
    emit currentCellDataTechnologyChanged(interface, tech);
}

void
SystemNetworkInfo::onCurrentMobileCountryCodeChanged(int interface, const QString& mcc) {
    DLOG(INFO) << " " << __PRETTY_FUNCTION__ << interface << mcc;
    emit currentMobileCountryCodeChanged(interface, mcc);
}

void
SystemNetworkInfo::onCurrentMobileNetworkCodeChanged(int interface, const QString& mnc) {
    DLOG(INFO) << " " << __PRETTY_FUNCTION__ << interface << mnc;
    emit currentMobileNetworkCodeChanged(interface, mnc);
}

void
SystemNetworkInfo::onCurrentNetworkModeChanged(QNetworkInfo::NetworkMode mode) {
    DLOG(INFO) << " " << __PRETTY_FUNCTION__ << mode;
    emit currentNetworkModeChanged(mode);
}

void
SystemNetworkInfo::onLocationAreaCodeChanged(int interface, const QString& lac) {
    DLOG(INFO) << " " << __PRETTY_FUNCTION__ << interface << lac;
    emit locationAreaCodeChanged(interface, lac);
}

void
SystemNetworkInfo::onNetworkInterfaceCountChanged(QNetworkInfo::NetworkMode mode,
                                                  int count) {
    DLOG(INFO) << " " << __PRETTY_FUNCTION__ << mode << count;
    emit networkInterfaceCountChanged(mode, count);
}

void
SystemNetworkInfo::onNetworkNameChanged(QNetworkInfo::NetworkMode mode, int interface,
                                        const QString& name) {
    DLOG(INFO) << " " << __PRETTY_FUNCTION__ << mode << interface << name;
    emit networkNameChanged(mode, interface, name);
}

void
SystemNetworkInfo::onNetworkSignalStrengthChanged(QNetworkInfo::NetworkMode mode,
                                                  int interface, int strength) {
    DLOG(INFO) << " " << __PRETTY_FUNCTION__ << mode << interface << strength;
    emit networkSignalStrengthChanged(mode, interface, strength);
}

void
SystemNetworkInfo::onNetworkStatusChanged(QNetworkInfo::NetworkMode mode, int interface,
                                          QNetworkInfo::NetworkStatus status) {
    DLOG(INFO) << " " << __PRETTY_FUNCTION__ << mode << interface << status;
    emit networkStatusChanged(mode, interface, status);
}

#endif

}  // System

}  // DownloadManager

}  // Ubuntu
