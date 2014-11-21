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

#include <QDBusConnection>
#include <glog/logging.h>

#include "network_session.h"

namespace {
    const QString NM_PATH = "org.freedesktop.NetworkManager";
    const QString NM_OBJ_PATH = "/org/freedesktop/NetworkManager";
    const QString NM_INTERFACE = "org.freedesktop.NetworkManager";
    const QString NM_PROPERTY = "PrimaryConnectionType";
}

namespace Ubuntu {

namespace Transfers {

namespace System {

NetworkSession* NetworkSession::_instance = nullptr;
QMutex NetworkSession::_mutex;

NetworkSession::NetworkSession(QObject* parent)
    : QObject(parent) {
    _configManager = new QNetworkConfigurationManager();

    CHECK(connect(_configManager, &QNetworkConfigurationManager::onlineStateChanged,
                this, &NetworkSession::onlineStateChanged))
             << "Could not connect to signal";

    _properties = new FreefreedesktopProperties(NM_PATH, NM_OBJ_PATH,
            QDBusConnection::systemBus());

    // get the property type for the very first time
    QDBusPendingReply<QDBusVariant> reply = _properties->Get(NM_INTERFACE, NM_PROPERTY);
    reply.waitForFinished();
    if (!reply.isError()) {
        qDebug() << "Connection type is " << reply.value().variant();
    }


    CHECK(connect(_properties, &FreefreedesktopProperties::PropertiesChanged,
                this, &NetworkSession::onPropertiesChanged))
             << "Could not connect to signal";
}

NetworkSession::~NetworkSession() {
    delete _properties;
    delete _configManager;
}

bool
NetworkSession::isOnline() {
    return _configManager->isOnline();
}

QNetworkConfiguration::BearerType
NetworkSession::sessionType() {
    return _sessionType;
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
NetworkSession::onPropertiesChanged(const QString& interfaceName,
                                    const QVariantMap& changedProperties,
                                    const QStringList& invalidatedProperties) {
    Q_UNUSED(invalidatedProperties);
    if (interfaceName == NM_INTERFACE) {
        if (changedProperties.contains(NM_PROPERTY)) {
        }

        foreach(const QString& key, changedProperties.keys()) {
            qDebug() << "Property changed " << key << changedProperties[key];
        }
    }
}

}

}

}
