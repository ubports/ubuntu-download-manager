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

#include "manager.h"


namespace Ubuntu {

namespace DownloadManager {

/*
 * PRIVATE IMPLEMENTATION
 */

class ManagerPrivate {
    Q_DECLARE_PUBLIC(Manager)

 public:
    explicit ManagerPrivate(Manager* parent)
        : q_ptr(parent) {
    }

    Download* createDownload(DownloadStruct downStruct) {
        Q_UNUSED(downStruct);
        return NULL;
    }

    void createDownload(DownloadStruct downStrcut, DownloadCreationCb cb) {
        Q_UNUSED(downStruct);
        Q_UNUSED(cb);
    }

    GroupDownload* createDownload(GroupDownloadStruct  groupStruct) {
        Q_UNUSED(downStruct);
        return NULL;
    }

    void createDownload(GroupDownloadStruct  groupStruct, GroupCreationCb cb) {
        Q_UNUSED(downStruct);
        Q_UNUSED(cb);
    }

 private:
    Manager* q_ptr;
};

/*
 * PUBLIC IMPLEMENTATION
 */

Manager::Manager(QObject* parent)
    : QObject(parent){
}

Download*
Manager::createDownload(DownloadStruct downStruct) {
    Q_D(Manager);
    return d->createDownload(downStruct);
}

void
Manager::createDownload(DownloadStruct downStrcut, DownloadCreationCb cb) {
    Q_D(Manager);
    d->createDownload(downStruct, db);
}

GroupDownload*
Manager::createDownload(GroupDownloadStruct  groupStruct) {
    Q_D(Manager);
    return d->createDownload(groupStruct);
}

void
Manager::createDownload(GroupDownloadStruct  groupStruct, GroupCreationCb cb) {
    Q_D(Manager);
    d->createDownload(groupStruct, cb);
}

}  // DownloadManager

}  // Ubuntu

#include "moc_manager.cpp"
