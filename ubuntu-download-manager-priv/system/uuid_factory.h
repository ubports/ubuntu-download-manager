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

#ifndef DOWNLOADER_LIB_UUID_FACTORY_H
#define DOWNLOADER_LIB_UUID_FACTORY_H

#include <QUuid>
#include <QObject>

namespace Ubuntu {

namespace DownloadManager {

namespace System {

class UuidFactory : public QObject {
    Q_OBJECT

 public:
    explicit UuidFactory(QObject *parent = 0);

    virtual QUuid createUuid();
};

}  // System

}  // DownloadManager

}  // Ubuntu
#endif  // DOWNLOADER_LIB_UUID_FACTORY_H
