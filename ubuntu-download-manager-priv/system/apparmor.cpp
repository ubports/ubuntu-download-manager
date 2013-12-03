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
#include <sys/types.h>
#include <unistd.h>
#include <QDBusConnection>
#include <QDebug>
#include <QDir>
#include <QRegExp>
#include <QStandardPaths>
#include "uuid_utils.h"
#include "logger.h"
#include "apparmor.h"

namespace Ubuntu {

namespace DownloadManager {

namespace System {

QString AppArmor::UNCONFINED_ID = "unconfined";

AppArmor::AppArmor(QObject* parent)
    : QObject(parent) {
    _dbus = new DBusProxy("org.freedesktop.DBus", "/",
        QDBusConnection::sessionBus(), this);
    _uuidFactory = new UuidFactory(this);
}

AppArmor::AppArmor(QSharedPointer<DBusConnection> connection,
                QObject* parent)
    : QObject(parent) {
    _dbus = new DBusProxy("org.freedesktop.DBus", "/",
        connection->connection(), this);
    _uuidFactory = new UuidFactory(this);
}

AppArmor::~AppArmor() {
    delete _dbus;
    delete _uuidFactory;
}

void
AppArmor::getDBusPath(QString& id, QString& dbusPath) {
    QUuid uuid = _uuidFactory->createUuid();
    id = UuidUtils::getDBusString(uuid);
    dbusPath = UuidUtils::getDBusString(uuid);
}

QString
AppArmor::getSecurePath(const QString& connName,
                    QString& dbusPath,
                    QString& localPath,
                    bool& isConfined) {
    QString id = UuidUtils::getDBusString(_uuidFactory->createUuid());
    getSecurePath(connName, id, dbusPath, localPath, isConfined);
    return id;
}

QString
AppArmor::getLocalPath(const QString& appId) {
    // if the service is running as root we will always return /tmp
    // as the local path root
    if (getuid() == 0){
        qDebug() << "Running as system bus using /tmp for downloads";
        return QStandardPaths::writableLocation(
            QStandardPaths::TempLocation);
    } else {
        QString dataPath = QStandardPaths::writableLocation(
            QStandardPaths::DataLocation);
        QStringList pathComponents;
        pathComponents << dataPath;

        if (!appId.isEmpty()) {
            QStringList appIdInfo = appId.split("_");
            if (appIdInfo.count() > 0)
                pathComponents << appIdInfo[0];
        }

        pathComponents << "Downloads";

        QString path = pathComponents.join(QDir::separator());

        bool wasCreated = QDir().mkpath(path);
        if (!wasCreated) {
            qCritical() << "Could not create the data path" << path;
        }
        qDebug() << "Local path is" << path;
        return path;
    }  // not root
}

void
AppArmor::getSecurePath(const QString& connName,
                   const QString& id,
                   QString& dbusPath,
                   QString& localPath,
                   bool& isConfined) {
    if (connName.isEmpty()) {
        dbusPath = QString(BASE_ACCOUNT_URL) + "/" + id;
        localPath = getLocalPath("");
        isConfined = false;
        return;
    }

    QDBusPendingReply<QString> reply =
        _dbus->GetConnectionAppArmorSecurityContext(connName);
    // blocking but should be ok for now
    reply.waitForFinished();
    if (reply.isError()) {
        qCritical() << reply.error();
        dbusPath = QString(BASE_ACCOUNT_URL) + "/" + id;
        localPath = getLocalPath("");
        isConfined = false;
        return;
    } else {
        // use the returned value
        QString appId = reply.value();

        if (appId.isEmpty() || appId == UNCONFINED_ID) {
            qDebug() << "UNCONFINED APP";
            dbusPath = QString(BASE_ACCOUNT_URL) + "/" + id;
            localPath = getLocalPath("");
            isConfined = false;
            return;
        } else {
            isConfined = true;
            QByteArray appIdBa = appId.toUtf8();

            char * appIdPath;
            appIdPath = nih_dbus_path(NULL, BASE_ACCOUNT_URL,
                appIdBa.data(), NULL);

            if (appIdPath == NULL) {
                qCritical() << "Unable to allocate memory for "
                    << "nih_dbus_path()";
                dbusPath = QString(BASE_ACCOUNT_URL) + "/" + id;
                localPath = getLocalPath(appId);
                return;
            }
            QString path = QString(appIdPath);
            qDebug() << "AppId path is " << appIdPath;

            // free nih data
            nih_free(appIdPath);
            dbusPath = path + "/" + id;
            localPath = getLocalPath(appId);
            return;
        }  // not empty appid string
    }  // no dbus error
}

}  // System

}  // DownloadManager

}  // Ubuntu
