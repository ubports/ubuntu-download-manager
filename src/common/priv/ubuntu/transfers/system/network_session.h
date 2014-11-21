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

#ifndef TRANSFERS_LIB_NETWORK_SESSION_H
#define TRANSFERS_LIB_NETWORK_SESSION_H

#include <QMutex>
#include <QNetworkConfigurationManager>
#include <QObject>

#include "properties_interface.h"

namespace Ubuntu {

namespace Transfers {

namespace System {

class NetworkSession : public QObject {
    Q_OBJECT

 public:
    ~NetworkSession();

    virtual bool isOnline();
    virtual QNetworkConfiguration::BearerType sessionType();

    static NetworkSession* instance();

    // only used for testing so that we can inject a fake
    static void setInstance(NetworkSession* instance);
    static void deleteInstance();

 signals:
    void sessionTypeChanged(QNetworkConfiguration::BearerType type);
    void onlineStateChanged(bool state);

 protected:
    explicit NetworkSession(QObject* parent=0);

 private:
    void onPropertiesChanged(const QString& interfaceName,
                             const QVariantMap& changedProperties,
                             const QStringList& invalidatedProperties);

 private:
    // used for the singleton
    static NetworkSession* _instance;
    static QMutex _mutex;

    FreefreedesktopProperties* _properties = nullptr;
    QNetworkConfigurationManager* _configManager = nullptr;
    QNetworkConfiguration::BearerType _sessionType =
        QNetworkConfiguration::BearerUnknown;
};

}

}

}

#endif
