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

using namespace General;

namespace UploadManager {

namespace Daemon {

const QString UploadManager::SERVICE_PATH;

UploadManager::UploadManager(Application* app,
                             DBusConnection* connection,
                             bool stoppable,
                             QObject *parent) {
}

UploadManager::UploadManager(Application* app,
                             DBusConnection* connection,
                             Factory* downloadFactory,
                             Queue* queue,
                             bool stoppable,
                             QObject *parent) {
}

UploadManager::~UploadManager() {
}

QList<QSslCertificate>
UploadManager::acceptedCertificates() {
}

void
UploadManager::setAcceptedCertificates(const QList<QSslCertificate>& certs) {
}

void
UploadManager::allowMobileUpload(bool allowed) {
}

QDBusObjectPath
UploadManager::createMmsUpload(const QString& url,
                               const QString& file,
                               const QString& hostname,
                               int port,
                               const QString& username,
                               const QString& password) {
}

QDBusObjectPath
UploadManager::createUpload(UploadStruct upload) {
}

qulonglong
UploadManager::defaultThrottle() {
}

QList<QDBusObjectPath>
UploadManager::getAllUploads() {
}

QList<QDBusObjectPath>
UploadManager::getAllUploadsWithMetadata(const QString& name,
                                         const QString& value) {
}

bool
UploadManager::isMobileUploadAllowed() {
}

void
UploadManager::setDefaultThrottle(qulonglong speed) {
}

}  // Daemon

}  // UploadManager

}  // Ubuntu
