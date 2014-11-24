/*
 * Copyright 2014 Canonical Ltd.
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

#include <ubuntu/transfers/system/logger.h>

#include "transfer.h"

namespace Ubuntu {

namespace Transfers {

Transfer::Transfer(const QString& id,
         const QString& appId,
         const QString& path,
         bool isConfined,
         const QString& rootPath,
         QObject* parent)
    : QObject(parent),
      _id(id),
      _appId(appId),
      _throttle(0),
      _allowMobileData(true),
      _state(Transfer::IDLE),
      _dbusPath(path),
      _isConfined(isConfined),
      _rootPath(rootPath) {
    _networkSession = System::NetworkSession::instance();
    setObjectName(id);
}

QString
Transfer::transferId() const {
    return _id;
}

QString
Transfer::transferAppId() const {
    return _appId;
}

QString
Transfer::path() const {
    return _dbusPath;
}

bool
Transfer::isConfined() const {
    return _isConfined;
}

QString
Transfer::rootPath() const {
    return _rootPath;
}


Transfer::State
Transfer::state() const {
    return _state;
}

void
Transfer::setState(Transfer::State state) {
    if (_state != state) {
        _state = state;
        emit stateChanged();
    }
}

bool
Transfer::canTransfer() {
    TRACE;
    auto mode = _networkSession->sessionType();
    switch (mode) {
        case QNetworkConfiguration::BearerUnknown:
            qWarning() << "Network Mode unknown!";
            return _allowMobileData;
            break;
        case QNetworkConfiguration::Bearer2G:
        case QNetworkConfiguration::Bearer3G:
        case QNetworkConfiguration::Bearer4G:
            return _allowMobileData;
        case QNetworkConfiguration::BearerEthernet:
        case QNetworkConfiguration::BearerWLAN:
        case QNetworkConfiguration::BearerBluetooth:
            return true;
        default:
            return false;
    }
}

bool
Transfer::isValid() const {
    return _isValid;
}

QString
Transfer::lastError() const {
    return _lastError;
}

bool
Transfer::addToQueue() const {
    return _addToQueue;
}

void
Transfer::setIsValid(bool isValid) {
    _isValid = isValid;
}

void
Transfer::setAddToQueue(bool addToQueue) {
    _addToQueue = addToQueue;
}

void
Transfer::setLastError(const QString& lastError) {
    _lastError = lastError;
}

void
Transfer::setThrottle(qulonglong speed) {
    if (speed != _throttle) {
        _throttle = speed;
        emit throttleChanged();
    }
}

qulonglong
Transfer::throttle() {
    return _throttle;
}

void
Transfer::allowGSMData(bool allowed) {
    if (_allowMobileData != allowed) {
        _allowMobileData = allowed;
        // emit the signals so that they q knows what to do
        emit stateChanged();
    }
}

bool
Transfer::isGSMDataAllowed() {
    return _allowMobileData;
}

void
Transfer::cancel() {
    setState(Transfer::CANCEL);
    if(!_addToQueue)
        cancelTransfer();
}

void
Transfer::pause() {
    setState(Transfer::PAUSE);
    if(!_addToQueue)
        pauseTransfer();
}

void
Transfer::resume() {
    setState(Transfer::RESUME);
    if(!_addToQueue)
        resumeTransfer();
}

void
Transfer::start() {
    setState(Transfer::START);
    if(!_addToQueue)
        startTransfer();
}

}  // General

}  // Ubuntu
