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
      _uuidFactory(uuidFactory) {
}

QPair<QUuid, QString>
FakeAppArmor::getSecurePath(QString connName) {
    QUuid id = _uuidFactory->createUuid();
    QString uuidString = id.toString().replace(QRegExp("[-{}]"), "");
    if (_recording) {
        QList<QObject*> inParams;
        inParams.append(new StringWrapper(connName));

        QList<QObject*> outParams;
        outParams.append(new UuidWrapper(id));
        outParams.append(new StringWrapper(uuidString));
        MethodParams params(inParams, outParams);
        MethodData methodData("getSecurePath", params);
        _called.append(methodData);
    }

    return QPair<QUuid, QString>(id, uuidString);
}
