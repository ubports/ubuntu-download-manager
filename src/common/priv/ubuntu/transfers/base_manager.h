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

#ifndef UBUNTU_GENERAL_MANAGER_H
#define UBUNTU_GENERAL_MANAGER_H

#include <QDBusContext>
#include <QList>
#include <QObject>
#include <QSslCertificate>
#include "ubuntu/transfers/system/application.h"

namespace Ubuntu {

namespace Transfers {

using namespace System;

class BaseManager : public QObject, public QDBusContext {
    Q_OBJECT

 public:
    BaseManager(Application* app,
                bool stopabble=false,
                QObject *parent = 0);

    virtual QList<QSslCertificate> acceptedCertificates() = 0;
    virtual void setAcceptedCertificates(
        const QList<QSslCertificate>& certs) = 0;

 public slots:
    virtual void exit();

 signals:
    void sizeChanged(int count);

 protected:
    Application* _app = nullptr;
    bool _stoppable = false;
};

}  // General

}  // Ubuntu

#endif // UBUNTU_GENERAL_MANAGER_H
