/*
 * Copyright 2014 Canonical Ltd.
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

#include "manager.h"

namespace Ubuntu {

using namespace Transfers;

namespace UploadManager {

namespace Daemon {

const QString UploadManager::SERVICE_PATH =
    "com.canonical.applications.Uploader";

UploadManager::UploadManager(Application* app,
                             DBusConnection* connection,
                             bool stoppable,
                             QObject *parent)
    : BaseManager(app, stoppable, parent) {
    Q_UNUSED(connection);
}

/*
UploadManager::UploadManager(Application* app,
                             DBusConnection* connection,
                             Factory* downloadFactory,
                             Queue* queue,
                             bool stoppable,
                             QObject *parent) {
}
*/

UploadManager::~UploadManager() {
}

QList<QSslCertificate>
UploadManager::acceptedCertificates() {
    // TODO: Most be implemented
    QList<QSslCertificate> certs;
    return certs;
}

void
UploadManager::setAcceptedCertificates(const QList<QSslCertificate>& certs) {
    // TODO: Most be implemented
    Q_UNUSED(certs);
}

void
UploadManager::allowMobileUpload(bool allowed) {
    // TODO: Most be implemented
    Q_UNUSED(allowed);
}

QDBusObjectPath
UploadManager::createMmsUpload(const QString& url,
                               const QString& file,
                               const QString& hostname,
                               int port,
                               const QString& username,
                               const QString& password) {
    // TODO: Most be implemented
    Q_UNUSED(url);
    Q_UNUSED(file);
    Q_UNUSED(hostname);
    Q_UNUSED(port);
    Q_UNUSED(username);
    Q_UNUSED(password);
    return QDBusObjectPath();
}

QDBusObjectPath
UploadManager::createUpload(UploadStruct upload) {
    // TODO: Most be implemented
    Q_UNUSED(upload);
    return QDBusObjectPath();
}

qulonglong
UploadManager::defaultThrottle() {
    // TODO: Most be implemented
    return 0;
}

QList<QDBusObjectPath>
UploadManager::getAllUploads() {
    // TODO: Most be implemented
    QList<QDBusObjectPath> uploads;
    return uploads;
}

QList<QDBusObjectPath>
UploadManager::getAllUploadsWithMetadata(const QString& name,
                                         const QString& value) {
    // TODO: Most be implemented
    Q_UNUSED(name);
    Q_UNUSED(value);
    QList<QDBusObjectPath> uploads;
    return uploads;
}

bool
UploadManager::isMobileUploadAllowed() {
    // TODO: Most be implemented
    return false;
}

void
UploadManager::setDefaultThrottle(qulonglong speed) {
    // TODO: Most be implemented
    Q_UNUSED(speed);
}

}  // Daemon

}  // UploadManager

}  // Ubuntu
