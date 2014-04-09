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

#ifndef UBUNTU_DOWNLOADMANAGER_CLIENT_DOWNLOAD_LIST_H
#define UBUNTU_DOWNLOADMANAGER_CLIENT_DOWNLOAD_LIST_H

#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <ubuntu/download_manager/common.h>

namespace Ubuntu {

namespace DownloadManager {

class Error;
class Download;

class DOWNLOAD_MANAGER_EXPORT DownloadsList : public QObject {
    Q_OBJECT

 public:
    explicit DownloadsList(QObject* parent = 0)
        : QObject(parent) {}

    virtual QList<QSharedPointer<Download> > downloads() const = 0;
    virtual bool isError() const = 0;
    virtual Error* error() const = 0;
};

}  // Ubuntu

}  // DownloadManager

#endif  // UBUNTU_DOWNLOADMANAGER_CLIENT_DOWNLOAD_LIST_H
