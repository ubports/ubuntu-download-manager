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

#include "apn_proxy.h"
#include "apn_request_factory.h"

namespace Ubuntu {

namespace DownloadManager {

namespace System {

ApnRequestFactory* ApnRequestFactory::_instance = NULL;
QMutex ApnRequestFactory::_mutex;

ApnRequestFactory::ApnRequestFactory(const QNetworkProxy& proxy,
                                     bool stoppable,
                                     QObject* parent)
    : RequestFactory(stoppable, parent) {
    _nam->setProxy(proxy);
}

ApnRequestFactory*
ApnRequestFactory::instance() {
    if(_instance == nullptr) {
        _mutex.lock();
        if(_instance == nullptr) {
            QScopedPointer<ApnProxy> apnProxy(new ApnProxy());
            _instance = new ApnRequestFactory(apnProxy->getProxy());
        }
        _mutex.unlock();
    }
    return _instance;
}

void
ApnRequestFactory::deleteInstance() {
    if(_instance != NULL) {
        _mutex.lock();
        if(_instance != NULL) {
            delete _instance;
            _instance = NULL;
        }
        _mutex.unlock();
    }
}

}  // System

}  // DownloadManager

}  // Ubuntu
