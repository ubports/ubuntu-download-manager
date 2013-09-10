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

#ifndef FAKE_APPARMOR_H
#define FAKE_APPARMOR_H

#include <QObject>
#include <QSharedPointer>
#include <apparmor.h>
#include <uuid_factory.h>
#include "./fake.h"

class FakeAppArmor : public AppArmor, public Fake {
    Q_OBJECT
 public:
    FakeAppArmor(QSharedPointer<UuidFactory> uuidFactory,
                 QObject *parent = 0);

    QPair<QUuid, QString> getSecurePath(QString connName) override;
    QPair<QUuid, QString> getSecurePath(QUuid id, QString connName) override;

 private:
    QSharedPointer<UuidFactory> _uuidFactory;
};

#endif  // FAKE_APPARMOR_H
