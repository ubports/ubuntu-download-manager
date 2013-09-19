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

#include "./fake_apparmor.h"

FakeAppArmor::FakeAppArmor(QSharedPointer<UuidFactory> uuidFactory,
                           QObject *parent)
    : AppArmor(parent),
      _isConfined(true),
      _uuidFactory(uuidFactory) {
}

void
FakeAppArmor::getDBusPath(QUuid& id, QString& dbusPath) {
    id = _uuidFactory->createUuid();
    dbusPath = id.toString().replace(QRegExp("[-{}]"), "");
    if (_recording) {
        QList<QObject*> inParams;

        QList<QObject*> outParams;
        outParams.append(new UuidWrapper(id));
        outParams.append(new StringWrapper(dbusPath));
        MethodParams params(inParams, outParams);
        MethodData methodData("getDBusPath", params);
        _called.append(methodData);
    }
}

QUuid
FakeAppArmor::getSecurePath(const QString& connName,
                        QString& dbusPath,
                        QString& localPath,
                        bool& isConfined) {
    QUuid id = _uuidFactory->createUuid();
    getSecurePath(connName, id, dbusPath, localPath, isConfined);
    return id;
}

void
FakeAppArmor::getSecurePath(const QString& connName,
                       const QUuid& id,
                       QString& dbusPath,
                       QString& localPath,
                       bool& isConfined) {
    QString uuidString = id.toString().replace(QRegExp("[-{}]"), "");
    dbusPath = "dbus/" + uuidString;
    localPath = "local/"+ uuidString;
    isConfined = _isConfined;
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new UuidWrapper(id));
        inParams.append(new StringWrapper(connName));

        QList<QObject*> outParams;
        outParams.append(new UuidWrapper(id));
        outParams.append(new StringWrapper(dbusPath));
        outParams.append(new StringWrapper(localPath));
        MethodParams params(inParams, outParams);
        MethodData methodData("getSecurePath", params);
        _called.append(methodData);
    }
}

bool
FakeAppArmor::isConfined() {
    return _isConfined;
}

void
FakeAppArmor::setIsConfined() {
    _isConfined = true;
}
