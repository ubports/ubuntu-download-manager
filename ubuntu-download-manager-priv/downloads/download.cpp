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

#include <QDebug>
#include <QStringList>
#include "system/logger.h"
#include "downloads/download.h"

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

Download::Download(const QString& id,
                   const QString& path,
                   bool isConfined,
                   const QString& rootPath,
                   const QVariantMap& metadata,
                   const QMap<QString, QString>& headers,
                   QObject* parent)
    : QObject(parent),
      _id(id),
      _throttle(0),
      _allowGSMDownload(true),
      _state(Download::IDLE),
      _dbusPath(path),
      _isConfined(isConfined),
      _rootPath(rootPath),
      _metadata(metadata),
      _headers(headers) {
    _networkInfo = SystemNetworkInfo::instance();
}

Download::~Download() {
    if (_adaptor != NULL) {
        _adaptor->deleteLater();
    }
}

void
Download::setState(Download::State state) {
    if (_state != state) {
        _state = state;
        emit stateChanged();
    }
}

void
Download::setAdaptor(QObject* adaptor) {
    _adaptor = adaptor;
}

bool
Download::canDownload() {
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

void
Download::setIsValid(bool isValid) {
    _isValid = isValid;
}

void
Download::setAddToQueue(bool addToQueue) {
    _addToQueue = addToQueue;
}

void
Download::setLastError(const QString& lastError) {
    _lastError = lastError;
}

void
Download::setThrottle(qulonglong speed) {
    _throttle = speed;
}

void
Download::allowGSMDownload(bool allowed) {
    if (_allowGSMDownload != allowed) {
        _allowGSMDownload = allowed;
        // emit the signals so that they q knows what to do
        emit stateChanged();
    }
}

bool
Download::isGSMDownloadAllowed() {
    return _allowGSMDownload;
}

void
Download::emitError(const QString& errorStr) {
    qDebug() << __PRETTY_FUNCTION__ << errorStr;
    setState(Download::ERROR);
    emit error(errorStr);
}

}  // Daemon

}  // DownloadManager

}  // Ubuntu
