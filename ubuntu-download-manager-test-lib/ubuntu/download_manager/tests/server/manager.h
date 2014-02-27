/*
 * Copyright 2013-2014 Canonical Ltd.
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

#ifndef FAKE_DOWNLOAD_MANAGER_H
#define FAKE_DOWNLOAD_MANAGER_H

#include <QObject>
#include <ubuntu/downloads/manager.h>
#include <ubuntu/transfers/manager_factory.h>
#include <ubuntu/transfers/tests/fake.h>

using namespace Ubuntu::Transfers;
using namespace Ubuntu::Transfers::Tests;
using namespace Ubuntu::System;
using namespace Ubuntu::DownloadManager::Daemon;

class FakeDownloadManager : public DownloadManager, public Fake {
    Q_OBJECT

 public:
    explicit FakeDownloadManager(Application* app,
                                 DBusConnection* connection,
                                 QObject *parent = 0);

    void emitSizeChaged(int size);
    QList<QSslCertificate> acceptedCertificates() override;
    void setAcceptedCertificates(const QList<QSslCertificate>& certs) override;
};

class FakeDownloadManagerFactory : public ManagerFactory {
    Q_OBJECT

 public:
    explicit FakeDownloadManagerFactory(QObject* parent = 0);
    FakeDownloadManagerFactory(FakeDownloadManager* man, QObject* parent=0);

    virtual BaseManager* createManager(Application* app,
                                   DBusConnection* connection,
                                   bool stoppable = false,
                                   QObject *parent = 0);
 private:
    FakeDownloadManager* _man = nullptr;
};

#endif  // FAKE_DOWNLOAD_MANAGER_H
