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

#ifndef DOWNLOADER_LIB_SYSTEM_NETWORK_INFO_H
#define DOWNLOADER_LIB_SYSTEM_NETWORK_INFO_H

#include <QNetworkInfo>
#include <QObject>

class SystemNetworkInfoPrivate;
class SystemNetworkInfo : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(SystemNetworkInfo)

 public:
    explicit SystemNetworkInfo(QObject *parent = 0);

    virtual QNetworkInfo::NetworkMode currentNetworkMode();

 signals:
    void currentNetworkModeChanged(QNetworkInfo::NetworkMode mode);

 private:
    Q_PRIVATE_SLOT(d_func(),
            void onCurrentNetworkModeChanged(QNetworkInfo::NetworkMode mode))

 private:
    SystemNetworkInfoPrivate* d_ptr;
};

#endif  // DOWNLOADER_LIB_SYSTEM_NETWORK_INFO_H
