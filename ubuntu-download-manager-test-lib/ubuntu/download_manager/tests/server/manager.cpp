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

#include "manager.h"

FakeDownloadManager::FakeDownloadManager(Application* app,
                                         DBusConnection* connection,
                                         QObject *parent)
    : Manager(app, connection, parent),
      Fake() {
}

void
FakeDownloadManager::emitSizeChaged(int size) {
    emit sizeChanged(size);
}

QList<QSslCertificate>
FakeDownloadManager::acceptedCertificates() {
    if (_recording) {
        QList<QObject*> inParams;
        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("acceptedCertificates", params);
        _called.append(methodData);
    }
    return Manager::acceptedCertificates();
}

void
FakeDownloadManager::setAcceptedCertificates(
                                const QList<QSslCertificate>& certs) {
    if (_recording) {
        QList<QObject*> inParams;
        QList<QObject*> outParams;
        MethodParams params(inParams, outParams);
        MethodData methodData("setAcceptedCertificates", params);
        _called.append(methodData);
    }
    Manager::setAcceptedCertificates(certs);
}
