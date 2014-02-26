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

#include <ubuntu/system/logger.h>
#include "transfer.h"

namespace Ubuntu {

namespace General {

Transfer::Transfer(const QString& id,
         const QString& path,
         bool isConfined,
         QObject* parent) {
}

QString
Transfer::transferId() const {
    return _id;
}

QString
Transfer::path() const {
    return _dbusPath;
}

bool
Transfer::isConfined() const {
    return _isConfined;
}

Transfer::State
Transfer::state() const {
    return _state;
}

void
Transfer::setState(Download::State state) {
    if (_state != state) {
        _state = state;
        emit stateChanged();
    }
}

bool
Transfer::canTransfer() {
    TRACE;
    QNetworkInfo::NetworkMode mode = _networkInfo->currentNetworkMode();
    switch (mode) {
        case QNetworkInfo::UnknownMode:
            qWarning() << "Network Mode unknown!";
            return _allowGSMDownload;
            break;
        case QNetworkInfo::GsmMode:
        case QNetworkInfo::CdmaMode:
        case QNetworkInfo::WcdmaMode:
        case QNetworkInfo::WimaxMode:
        case QNetworkInfo::TdscdmaMode:
        case QNetworkInfo::LteMode:
            return _allowGSMDownload;
        case QNetworkInfo::WlanMode:
        case QNetworkInfo::EthernetMode:
        case QNetworkInfo::BluetoothMode:
            return true;
        default:
            return false;
    }
}

bool
Transfer::isValid() const {
    return _isValid;
}

bool
Transfer::addToQueue() const {
    return _addToQueue;
}

void
Transfer::setThrottle(qulonglong speed) {
}

qulonglong
Transfer::throttle() {
    return _throttle;
}

void
Transfer::allowGSMDownload(bool allowed) {
}

bool
Transfer::isGSMDownloadAllowed() {
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
