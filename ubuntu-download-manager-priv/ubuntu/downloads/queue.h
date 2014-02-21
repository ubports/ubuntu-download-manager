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

#ifndef DOWNLOADER_LIB_DOWNLOAD_QUEUE_H
#define DOWNLOADER_LIB_DOWNLOAD_QUEUE_H

#include <QList>
#include <QPair>
#include <QSharedPointer>
#include "download.h"
#include "download_adaptor.h"

namespace Ubuntu {

namespace DownloadManager {

using namespace System;

namespace Daemon {

class Queue : public QObject {
    Q_OBJECT

 public:
    explicit Queue(QObject* parent = 0);

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
    void onManagedDownloadStateChanged();
    void onUnmanagedDownloadStateChanged();
    void onCurrentNetworkModeChanged(QNetworkInfo::NetworkMode mode);
    void remove(const QString& path);
    void updateCurrentDownload();

 private:
    QString _current;
    QHash<QString, Download*> _downloads;  // quick for access
    QStringList _sortedPaths;  // keep the order
};

}  // Daemon

}  // DownloadManager

}  // Ubuntu

#endif  // DOWNLOADER_LIB_DOWNLOAD_QUEUE_H
