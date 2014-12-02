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
#include <QDir>
#include <QRegExp>
#include <QStandardPaths>
#include <ubuntu/transfers/system/logger.h>
#include "apparmor.h"
#include "dbus_proxy_factory.h"
#include "pending_reply.h"
#include "uuid_utils.h"

namespace Ubuntu {

namespace Transfers {

namespace System {

QString AppArmor::UNCONFINED_ID = "unconfined";

AppArmor::AppArmor(DBusConnection* connection,
                QObject* parent)
    : QObject(parent) {
    _dbus = DBusProxyFactory::instance()->createDBusProxy(connection, this);
    _uuidFactory = new UuidFactory(this);
}

AppArmor::~AppArmor() {
    delete _dbus;
    delete _uuidFactory;
}

SecurityDetails*
AppArmor::getSecurityDetails(const QString& connName) {
    auto id = UuidUtils::getDBusString(_uuidFactory->createUuid());
    auto details = new SecurityDetails(id);
    getSecurityDetails(connName, details);
    return details;
}

SecurityDetails*
AppArmor::getSecurityDetails(const QString& connName,
                             const QString& id) {

    auto details = new SecurityDetails(id);
    getSecurityDetails(connName, details);
    return details;
}

QString
AppArmor::appId(QString caller) {
    QScopedPointer<PendingReply<QString> > reply(
        _dbus->GetConnectionAppArmorSecurityContext(caller));
    // blocking but should be ok for now
    reply->waitForFinished();
    if (reply->isError()) {
        return "";
    }
    return reply->value();
}

bool
AppArmor::isConfined(QString appId) {
    if (appId.isEmpty() || appId == UNCONFINED_ID) {
        return false;
    }
    return true;
}

void
AppArmor::getSecurityDetails(const QString& connName,
                             SecurityDetails* details) {
    if (connName.isEmpty()) {
        details->dbusPath = QString(BASE_ACCOUNT_URL) + "/" + details->id;
        details->localPath = getLocalPath("");
        details->isConfined = false;
        return;
    }

    QScopedPointer<PendingReply<QString> > reply (
        _dbus->GetConnectionAppArmorSecurityContext(connName));
    // blocking but should be ok for now
    reply->waitForFinished();
    if (reply->isError()) {
        LOG(ERROR) << reply->error();
        details->dbusPath = QString(BASE_ACCOUNT_URL) + "/" + details->id;
        details->localPath = getLocalPath("");
        details->isConfined = false;
        return;
    } else {
        // use the returned value
        details->appId = reply->value();

        if (details->appId.isEmpty() || details->appId == UNCONFINED_ID) {
            LOG(INFO) << "UNCONFINED APP";
            details->dbusPath = QString(BASE_ACCOUNT_URL) + "/" + details->id;
            details->localPath = getLocalPath("");
            details->isConfined = false;
            return;
        } else {
            details->isConfined = true;
            QByteArray appIdBa = details->appId.toUtf8();

            char * appIdPath;
            appIdPath = nih_dbus_path(nullptr, BASE_ACCOUNT_URL,
                appIdBa.data(), nullptr);

            if (appIdPath == nullptr) {
                LOG(ERROR) << "Unable to allocate memory for "
                    << "nih_dbus_path()";
                details->dbusPath = QString(BASE_ACCOUNT_URL) + "/" + details->id;
                details->localPath = getLocalPath(details->appId);
                return;
            }
            QString path = QString(appIdPath);
            LOG(INFO) << "AppId path is " << appIdPath;

            // free nih data
            nih_free(appIdPath);
            details->dbusPath = path + "/" + details->id;
            details->localPath = getLocalPath(details->appId);
            return;
        }  // not empty appid string
    }  // no dbus error
}

QPair<QString, QString>
AppArmor::getDBusPath() {
    QUuid uuid = _uuidFactory->createUuid();
    QString id = UuidUtils::getDBusString(uuid);
    QString dbusPath = UuidUtils::getDBusString(uuid);
    return QPair<QString, QString>(id, dbusPath);
}

QString
AppArmor::getLocalPath(const QString& appId) {
    // if the service is running as root we will always return /tmp
    // as the local path root
    if (getuid() == 0){
        LOG(INFO) << "Running as system bus using /tmp for downloads";
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
            LOG(ERROR) << "Could not create the data path"
                << path;
        }
        LOG(INFO) << "Local path is" << path;
        return path;
    }  // not root
}

}  // System

}  // Transfers

}  // Ubuntu
