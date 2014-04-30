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
#include <glog/logging.h>
#include <ubuntu/transfers/system/logger.h>
#include "system_network_info.h"

namespace Ubuntu {

namespace System {

SystemNetworkInfo* SystemNetworkInfo::_instance = nullptr;
QMutex SystemNetworkInfo::_mutex;

SystemNetworkInfo::SystemNetworkInfo(QObject* parent)
    : QObject(parent) {
    _info = new QNetworkInfo(this);
    _configMan = new QNetworkConfigurationManager(this);

#ifdef DEBUG
    // in debug do log the changes else just fwd them
    CHECK(connect(_info, &QNetworkInfo::cellIdChanged, this,
        &SystemNetworkInfo::onCellIdChanged))
                    << "Could not connect to signal";
    CHECK(connect(_info, &QNetworkInfo::currentCellDataTechnologyChanged,
        this, &SystemNetworkInfo::onCurrentCellDataTechnologyChanged))
                    << "Could not connect to signal";
    CHECK(connect(_info, &QNetworkInfo::currentMobileCountryCodeChanged, this,
        &SystemNetworkInfo::onCurrentMobileCountryCodeChanged))
                    << "Could not connect to signal";
    CHECK(connect(_info, &QNetworkInfo::currentMobileNetworkCodeChanged, this,
        &SystemNetworkInfo::onCurrentMobileNetworkCodeChanged))
                    << "Could not connect to signal";
    CHECK(connect(_info, &QNetworkInfo::currentNetworkModeChanged, this,
        &SystemNetworkInfo::onCurrentNetworkModeChanged))
                    << "Could not connect to signal";
    CHECK(connect(_info, &QNetworkInfo::locationAreaCodeChanged, this,
        &SystemNetworkInfo::onLocationAreaCodeChanged))
                    << "Could not connect to signal";
    CHECK(connect(_info, &QNetworkInfo::networkInterfaceCountChanged, this,
        &SystemNetworkInfo::onNetworkInterfaceCountChanged))
                    << "Could not connect to signal";
    CHECK(connect(_info, &QNetworkInfo::networkNameChanged, this,
        &SystemNetworkInfo::onNetworkNameChanged))
                    << "Could not connect to signal";
    CHECK(connect(_info, &QNetworkInfo::networkSignalStrengthChanged, this,
        &SystemNetworkInfo::onNetworkSignalStrengthChanged))
                    << "Could not connect to signal";
    CHECK(connect(_info, &QNetworkInfo::networkStatusChanged, this,
        &SystemNetworkInfo::onNetworkStatusChanged))
                    << "Could not connect to signal";

    CHECK(connect(_configMan,
        &QNetworkConfigurationManager::onlineStateChanged, this,
        &SystemNetworkInfo::onOnlineStateChanged))
                    << "Could not connect to signal";
#else
    // connect to interesting signals
    CHECK(connect(_info, &QNetworkInfo::cellIdChanged, this,
        &SystemNetworkInfo::cellIdChanged))
                    << "Could not connect to signal";
    CHECK(connect(_info, &QNetworkInfo::currentCellDataTechnologyChanged,
        this, &SystemNetworkInfo::currentCellDataTechnologyChanged))
                    << "Could not connect to signal";
    CHECK(connect(_info, &QNetworkInfo::currentMobileCountryCodeChanged, this,
        &SystemNetworkInfo::currentMobileCountryCodeChanged))
                    << "Could not connect to signal";
    CHECK(connect(_info, &QNetworkInfo::currentMobileNetworkCodeChanged, this,
        &SystemNetworkInfo::currentMobileNetworkCodeChanged))
                    << "Could not connect to signal";
    CHECK(connect(_info, &QNetworkInfo::currentNetworkModeChanged, this,
        &SystemNetworkInfo::currentNetworkModeChanged))
                    << "Could not connect to signal";
    CHECK(connect(_info, &QNetworkInfo::locationAreaCodeChanged, this,
        &SystemNetworkInfo::locationAreaCodeChanged))
                    << "Could not connect to signal";
    CHECK(connect(_info, &QNetworkInfo::networkInterfaceCountChanged, this,
        &SystemNetworkInfo::networkInterfaceCountChanged))
                    << "Could not connect to signal";
    CHECK(connect(_info, &QNetworkInfo::networkNameChanged, this,
        &SystemNetworkInfo::networkNameChanged))
                    << "Could not connect to signal";
    CHECK(connect(_info, &QNetworkInfo::networkSignalStrengthChanged, this,
        &SystemNetworkInfo::networkSignalStrengthChanged))
                    << "Could not connect to signal";
    CHECK(connect(_info, &QNetworkInfo::networkStatusChanged, this,
        &SystemNetworkInfo::networkStatusChanged))
                    << "Could not connect to signal";

    CHECK(connect(_configMan,
        &QNetworkConfigurationManager::onlineStateChanged, this,
        &SystemNetworkInfo::onlineStateChanged))
                    << "Could not connect to signal";
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
    if(_instance == nullptr) {
        LOG(INFO) << "Instance is null";
        _mutex.lock();
        if(_instance == nullptr){
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
    if(_instance != nullptr) {
        _mutex.lock();
        if(_instance != nullptr) {
            delete _instance;
            _instance = nullptr;
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

}  // Ubuntu
