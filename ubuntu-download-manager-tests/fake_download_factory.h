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
#include <download_factory.h>
#include "./fake.h"

class FakeDownloadFactory : public DownloadFactory, public Fake {
    Q_OBJECT

 public:
    FakeDownloadFactory(SystemNetworkInfo* networkInfo,
                        RequestFactory* nam,
                        ProcessFactory* processFactory,
                        QObject *parent = 0);

    SingleDownload* createDownload(const QUuid& id,
                                   const QString& path,
                                   const QUrl& url,
                                   const QVariantMap& metadata,
                                   const QMap<QString, QString>& headers);

    SingleDownload* createDownload(const QUuid& id,
                                   const QString& path,
                                   const QUrl& url,
                                   const QString& hash,
                                   QCryptographicHash::Algorithm algo,
                                   const QVariantMap& metadata,
                                   const QMap<QString, QString>& headers);

    QList<SingleDownload*> downloads();

 private:
    SystemNetworkInfo* _networkInfo;
    RequestFactory* _nam;
    ProcessFactory* _processFactory;
    QList<SingleDownload*> _downloads;
};

#endif  // FAKE_DOWNLOAD_FACTORY_H
