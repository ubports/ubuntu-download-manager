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

#ifndef DOWNLOADER_LIB_APP_ARMOR_H
#define DOWNLOADER_LIB_APP_ARMOR_H

#include <QObject>
#include <QPair>
#include <QString>
#include <QSharedPointer>
#include <ubuntu/transfers/system/dbus_connection.h>
#include "dbus_proxy.h"
#include "uuid_factory.h"


namespace Ubuntu {

namespace Transfers {

namespace System {

struct SecurityDetails {
 public:
    SecurityDetails() = default;
    SecurityDetails(const QString& requiredId) { id = requiredId; }
    QString id;
    QString appId;
    QString dbusPath;
    QString localPath;
    bool isConfined = false;
};

class AppArmor : public QObject {
    Q_OBJECT

 public:
    explicit AppArmor(QObject *parent = 0);
    AppArmor(QSharedPointer<DBusConnection> connection, QObject *parent = 0);
    ~AppArmor();

    virtual QPair<QString, QString> getDBusPath();

    virtual SecurityDetails* getSecurityDetails(const QString& connName);
    virtual SecurityDetails* getSecurityDetails(const QString& connName,
                                                const QString& id);
    virtual QString appId(QString caller);
    virtual bool isConfined(QString appId);

    static QString UNCONFINED_ID;

 private:
    void getSecurityDetails(const QString& connName,
                            SecurityDetails* details);
    QString getLocalPath(const QString& appId);

 private:
    const char* BASE_ACCOUNT_URL = "/com/canonical/applications/%1";

    DBusProxy* _dbus;
    UuidFactory* _uuidFactory;
};

}  // System

}  // Transfers

}  // Ubuntu

#endif  // DOWNLOADER_LIB_APP_ARMOR_H
