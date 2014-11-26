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
#include <QScopedPointer>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>

#include <glog/logging.h>

#include "network_session.h"

namespace {
    const QString NM_PATH = "org.freedesktop.NetworkManager";
    const QString NM_OBJ_PATH = "/org/freedesktop/NetworkManager";
    const QString NM_INTERFACE = "org.freedesktop.NetworkManager";
    const QString PROPERTIES_INTERFACE = "org.freedesktop.DBus.Properties";
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

    _nm = new NMInterface(NM_PATH, NM_OBJ_PATH, QDBusConnection::systemBus());

    CHECK(connect(_nm, &NMInterface::PropertiesChanged,
        this, &NetworkSession::onPropertiesChanged))
	    << "Could not connect to signal";

    QScopedPointer<QDBusInterface> interface(new QDBusInterface(NM_PATH, NM_OBJ_PATH,
        PROPERTIES_INTERFACE, QDBusConnection::systemBus()));

    QDBusPendingReply<QDBusVariant> reply =
        interface->call("Get", NM_INTERFACE, NM_PROPERTY);

    reply.waitForFinished();
    if (!reply.isError()) {
	LOG(INFO) << "Connection type " << reply.value().variant().toString().toStdString();
        _sessionType = convertNMString(reply.value().variant().toString());
    } else {
	_error = true;
	_errorMsg = reply.error().message();
	LOG(ERROR) << _errorMsg.toStdString();
    }

}

NetworkSession::~NetworkSession() {
    delete _configManager;
}

bool
NetworkSession::isError() {
    return _error;
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

QNetworkConfiguration::BearerType
NetworkSession::convertNMString(const QString& str) {
    if (str.contains("wireless")) {
        return QNetworkConfiguration::BearerWLAN;
    }

    if (str.contains("ethernet")) {
        return QNetworkConfiguration::BearerEthernet;
    }

    if (str == "gsm") {
        return QNetworkConfiguration::Bearer3G;
    }
    return QNetworkConfiguration::BearerUnknown;
}

void
NetworkSession::onPropertiesChanged(const QVariantMap& changedProperties) {
    if (changedProperties.contains(NM_PROPERTY)) {
	auto nmStr = changedProperties[NM_PROPERTY].toString();
	LOG(INFO) << "Connection type " << nmStr.toStdString();
	auto type = convertNMString(nmStr);
	if (type != _sessionType) {
            _sessionType = type;
            emit sessionTypeChanged(type);
	}
    }

}

}

}

}
