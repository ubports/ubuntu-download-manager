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
#include "dbus_proxy.h"
#include "uuid_factory.h"
#include "logger.h"
#include "apparmor.h"

/*
 * PRIVATE IMPLEMENTATION
 */

class AppArmorPrivate {
    Q_DECLARE_PUBLIC(AppArmor)

 public:
    AppArmorPrivate(AppArmor* parent)
        : q_ptr(parent) {
        _dbus = new DBusProxy("org.freedesktop.DBus", "/",
            QDBusConnection::sessionBus());
        _uuidFactory = new UuidFactory();
    }

    AppArmorPrivate(QSharedPointer<DBusConnection> connection,
                    AppArmor* parent)
        : q_ptr(parent) {
        _dbus = new DBusProxy("org.freedesktop.DBus", "/",
            connection->connection());
        _uuidFactory = new UuidFactory();
    }

    QString getUuidPath(QUuid id, QString path) {
        TRACE << path;
        QString idString = path + "/" + id.toString().replace(
            QRegExp("[-{}]"), "");
        qDebug() << "Download path is" << idString;
        return idString;
    }

    void getDBusPath(QUuid& id, QString& dbusPath) {
        id = _uuidFactory->createUuid();
        dbusPath = getUuidPath(id, "");
    }

    QUuid getSecurePath(const QString& connName,
                        QString& dbusPath,
                        QString& localPath,
                        bool& isConfined) {
        QUuid id = _uuidFactory->createUuid();
        getSecurePath(connName, id, dbusPath, localPath, isConfined);
        return id;
    }

    QString getLocalPath(const QString& appId) {
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

    void getSecurePath(const QString& connName,
                       const QUuid& id,
                       QString& dbusPath,
                       QString& localPath,
                       bool& isConfined) {
        if (connName.isEmpty()) {
            dbusPath = getUuidPath(id, QString(BASE_ACCOUNT_URL));
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
            dbusPath = getUuidPath(id, QString(BASE_ACCOUNT_URL));
            localPath = getLocalPath("");
            isConfined = false;
            return;
        } else {
            // use the returned value
            QString appId = reply.value();

            if (appId.isEmpty() || appId == UNCONFINED_ID) {
                qDebug() << "UNCONFINED APP";
                dbusPath = getUuidPath(id, QString(BASE_ACCOUNT_URL));
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
                    dbusPath = getUuidPath(id, QString(BASE_ACCOUNT_URL));
                    localPath = getLocalPath(appId);
                    return;
                }
                QString path = QString(appIdPath);
                qDebug() << "AppId path is " << appIdPath;

                // free nih data
                nih_free(appIdPath);
                dbusPath = getUuidPath(id, path);
                localPath = getLocalPath(appId);
                return;
            }  // not empty appid string
        }  // no dbus error
    }

 private:
    const char* BASE_ACCOUNT_URL = "/com/canonical/applications/download";
    static QString UNCONFINED_ID;

    DBusProxy* _dbus;
    UuidFactory* _uuidFactory;
    AppArmor* q_ptr;
};

QString AppArmorPrivate::UNCONFINED_ID = "unconfined";

/*
 * PUBLIC IMPLEMENTATION
 */


AppArmor::AppArmor(QObject *parent)
    : QObject(parent),
      d_ptr(new AppArmorPrivate(this)) {
}

AppArmor::AppArmor(QSharedPointer<DBusConnection> connection,
                   QObject *parent)
    : QObject(parent),
      d_ptr(new AppArmorPrivate(connection, this)) {
}

void
AppArmor::getDBusPath(QUuid& id, QString& dbusPath) {
    Q_D(AppArmor);
    return d->getDBusPath(id, dbusPath);
}

QUuid
AppArmor::getSecurePath(const QString& connName,
                        QString& dbusPath,
                        QString& localPath,
                        bool& isConfined) {
    Q_D(AppArmor);
    return d->getSecurePath(connName, dbusPath, localPath, isConfined);
}

void
AppArmor::getSecurePath(const QString& connName,
                        const QUuid& id,
                        QString& dbusPath,
                        QString& localPath,
                        bool& isConfined) {
    Q_D(AppArmor);
    d->getSecurePath(connName, id, dbusPath, localPath, isConfined);
}
