/*
 * Copyright 2013 Canonical Ltd.
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

#include <errno.h>
#include <nih/alloc.h>
#include <libnih-dbus.h>
#include <QDBusConnection>
#include <QDebug>
#include <QRegExp>
#include "./dbus_proxy.h"
#include "./uuid_factory.h"
#include "./apparmor.h"

/*
 * PRIVATE IMPLEMENTATION
 */

class AppArmorPrivate {
    Q_DECLARE_PUBLIC(AppArmor)

 public:
    explicit AppArmorPrivate(AppArmor* parent)
        : q_ptr(parent) {
        _dbus = new DBusProxy("org.freedesktop.DBus", "/",
            QDBusConnection::sessionBus());
        _uuidFactory = new UuidFactory();
    }

    QString getUuidString() {
        QUuid id = _uuidFactory->createUuid();
        return id.toString().replace(QRegExp("[-{}]"), "");
    }

    QString getSecurePath(QString connectionName) {
        QDBusPendingReply<QString> reply =
            _dbus->GetConnectionAppArmorSecurityContext(connectionName);
        // blocking but should be ok for now
        reply.waitForFinished();
        if (reply.isError()) {
            qCritical() << reply.error();
            return QString(BASE_ACCOUNT_URL) + "/" + getUuidString();
        } else {
            // use the returned value
            QString appId = reply.value();
            qDebug() << "AppId is " << appId;

            QByteArray appIdBa = appId.toUtf8();

            char * appIdPath;
            appIdPath = nih_dbus_path(NULL, AppArmorPrivate::BASE_ACCOUNT_URL,
                appIdBa.data(), NULL);

            if (appIdPath == NULL) {
                qCritical() << "Unable to allocate memory for nih_dbus_path()";
                return QString(BASE_ACCOUNT_URL) + "/" + getUuidString();
            }
            QString path = QString(appIdPath);
            qDebug() << "AppId path is " << appIdPath;

            // free nih data
            nih_free(appIdPath);
            return path + "/" + getUuidString();
        }
    }

 private:
    const char* BASE_ACCOUNT_URL = "/com/canonical/applications/download";

    DBusProxy* _dbus;
    UuidFactory* _uuidFactory;
    AppArmor* q_ptr;
};

/*
 * PUBLIC IMPLEMENTATION
 */


AppArmor::AppArmor(QObject *parent)
    : QObject(parent),
      d_ptr(new AppArmorPrivate(this)) {
}

QString
AppArmor::getSecurePath(QString connectionName) {
    Q_D(AppArmor);
    return d->getSecurePath(connectionName);
}
