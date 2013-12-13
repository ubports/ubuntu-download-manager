/*
 * Copyright 2013 2013 Canonical Ltd.
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

#ifndef FAKE_DOWNLOAD_FACTORY_H
#define FAKE_DOWNLOAD_FACTORY_H

#include <QObject>
#include <QSharedPointer>
#include <downloads/factory.h>
#include "ubuntu/download_manager/tests/fake.h"

using namespace Ubuntu::DownloadManager::System;
using namespace Ubuntu::DownloadManager::Daemon;

class FakeDownloadFactory : public Factory, public Fake {
    Q_OBJECT

 public:
    FakeDownloadFactory(AppArmor* apparmor,
                        QObject *parent = 0);

    Download* createDownload(const QString& downloadOwner,
                             const QUrl& url,
                             const QVariantMap& metadata,
                             const QMap<QString, QString>& headers) override;

    Download* createDownload(const QString& downloadOwner,
                             const QUrl& url,
                             const QString& hash,
                             const QString& algo,
                             const QVariantMap& metadata,
                             const QMap<QString, QString>& headers) override;

    Download* createDownload(const QString& downloadOwner,
                             StructList downloads,
                             const QString& algo,
                             bool allowed3G,
                             const QVariantMap& metadata,
                             StringMap headers) override;

    Download* createDownloadForGroup(bool isConfined,
                             const QString& rootPath,
                             const QUrl& url,
                             const QVariantMap& metadata,
                             const QMap<QString, QString>& headers) override;

    Download* createDownloadForGroup(bool isConfined,
                             const QString& rootPath,
                             const QUrl& url,
                             const QString& hash,
                             const QString& algo,
                             const QVariantMap& metadata,
                             const QMap<QString, QString>& headers) override;

    QList<Download*> downloads();

 private:
    AppArmor* _apparmor;
    QList<Download*> _downloads;
};

#endif  // FAKE_DOWNLOAD_FACTORY_H
