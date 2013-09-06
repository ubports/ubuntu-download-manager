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
#include <sys/apparmor.h>
#include <nih/alloc.h>
#include <libnih-dbus.h>
#include <QRegExp>
#include <QDebug>
#include "./uuid_factory.h"
#include "./app_armor.h"

/*
 * PRIVATE IMPLEMENTATION
 */

class AppArmorPrivate {
    Q_DECLARE_PUBLIC(AppArmor)

 public:
    explicit AppArmorPrivate(AppArmor* parent)
        : q_ptr(parent) {
        _uuidFactory = new UuidFactory();
    }

    QString getUuidString() {
        QUuid id = _uuidFactory->createUuid();
        return id.toString().replace(QRegExp("[-{}]"), "");
    }

    QString getSecurePath(uint pid) {
        char* appId = NULL;
        char* mode = NULL;

        int success = aa_gettaskcon(pid, &appId, &mode);
        if (success == EINVAL) {
            qCritical() << "The apparmor kernel module is not loaded or the "
                << "communication via the /proc/*/attr/file did not conform "
                << "to protocol.";
        } else if (success == ENOMEM) {
            qCritical() << "Insufficient kernel memory was available.";
        } else if (success ==  EACCES) {
            qCritical() << "Access to the specified file/task was denied.";
        } else if (success == ENOENT) {
            qCritical() << "The specified file/task does not exist or is "
                << "not visible.";
        } else if (success == ERANGE) {
            qCritical() << "The confinement data is too large to fit in the "
                << "supplied buffer.";
        } else {
            // check create a QString for the app id and make it usuable for dbus
            qDebug() << "AppId is " << appId;

            char * appIdPath;
            appIdPath = nih_dbus_path(NULL, AppArmorPrivate::BASE_ACCOUNT_URL , appId, NULL);

            if (appIdPath == NULL) {
                qCritical() << "Unable to allocate memory for nih_dbus_path()";
                free(appId);
                free(mode);
                return QString(BASE_ACCOUNT_URL) + "/" + getUuidString();
            }
            QString path = QString(appIdPath);
            qDebug() << "AppId path is " << appIdPath;

            // fre nih data
            nih_free(appIdPath);
            // free app armor used data
            free(appId);
            free(mode);
            return path + "/" + getUuidString();
        }
        return QString(BASE_ACCOUNT_URL) + "/" + getUuidString();
    }

 private:
    const char* BASE_ACCOUNT_URL = "/com/canonical/applications/download";

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
AppArmor::getSecurePath(uint pid) {
    Q_D(AppArmor);
    return d->getSecurePath(pid);
}
