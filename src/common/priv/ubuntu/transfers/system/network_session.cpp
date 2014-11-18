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

#include <glog/logging.h>

#include "network_session.h"

namespace Ubuntu {

namespace Transfers {

namespace System {

NetworkSession* NetworkSession::_instance = nullptr;
QMutex NetworkSession::_mutex;

NetworkSession::NetworkSession(QObject* parent)
    : QObject(parent) {
    _configManager = new QNetworkConfigurationManager();
    _session = new QNetworkSession(_configManager->defaultConfiguration());
    // connect to the default config changed signal, that way we can emit that the
    // connection type changed and perform the migration.
    CHECK(connect(_session, &QNetworkSession::preferredConfigurationChanged, this,
        &NetworkSession::onDefaultConfigurationChanged))
             << "Could not connect to signal";

    CHECK(connect(_configManager, &QNetworkConfigurationManager::onlineStateChanged,
                this, &NetworkSession::onlineStateChanged))
             << "Could not connect to signal";
    _session->open();
    _session->waitForOpened();
}

NetworkSession::~NetworkSession() {
    delete _configManager;
    delete _session;
}

bool
NetworkSession::isOnline() {
    return _configManager->isOnline();
}

QNetworkConfiguration::BearerType
NetworkSession::sessionType() {
    return _session->configuration().bearerTypeFamily();
}

NetworkSession*
NetworkSession::instance() {
    if(_instance == nullptr) {
        LOG(INFO) << "Instance is null";
        _mutex.lock();
        if(_instance == nullptr){
            LOG(INFO) << "Create new instance";
            _instance = new NetworkSession();
        }
        _mutex.unlock();
    }
    return _instance;
}

void
NetworkSession::setInstance(NetworkSession* instance) {
    _instance = instance;
}

void
NetworkSession::deleteInstance() {
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
NetworkSession::onDefaultConfigurationChanged(const QNetworkConfiguration& config,
                                              bool isSeamless) {
    LOG(INFO) << "Connection default configuration changed type is " << config.bearerType();

    // as an application, we alwayes perform the migration
    emit sessionTypeChanged(config.bearerTypeFamily());
    _session->migrate();

    if (!isSeamless) {
        LOG(INFO) << "Transition is not seamless";
        // TODO: We need to test if it is required to pause all downloads and
        // resume them accordingly, probably atm only the current one needs to deal
        // with this use case.
    }
}

}

}

}
