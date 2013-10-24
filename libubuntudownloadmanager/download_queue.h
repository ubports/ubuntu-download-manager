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

#ifndef DOWNLOADER_LIB_DOWNLOAD_QUEUE_H
#define DOWNLOADER_LIB_DOWNLOAD_QUEUE_H

#include <QList>
#include <QPair>
#include <QSharedPointer>
#include "download.h"
#include "download_adaptor.h"
#include "system/system_network_info.h"

class DownloadQueuePrivate;
class DownloadQueue : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(DownloadQueue)

 public:
    explicit DownloadQueue(QSharedPointer<SystemNetworkInfo> networkInfo,
                           QObject* parent = 0);

    virtual void add(Download* download);

    // accessors for useful info
    QString currentDownload();
    QStringList paths();
    QHash<QString, Download*> downloads();
    virtual int size();

 signals:
    // signals raised when things happens within the q
    void currentChanged(QString path);
    void downloadAdded(QString path);
    void downloadRemoved(QString path);

 private:
    Q_PRIVATE_SLOT(d_func(), void onDownloadStateChanged())
    Q_PRIVATE_SLOT(d_func(),
            void onCurrentNetworkModeChanged(QNetworkInfo::NetworkMode mode))

 private:
    DownloadQueuePrivate* d_ptr;
};

#endif  // DOWNLOADER_LIB_DOWNLOAD_QUEUE_H
