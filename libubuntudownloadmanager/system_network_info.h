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

#include <QNetworkAccessManager>
#include <QNetworkInfo>
#include <QObject>

class SystemNetworkInfoPrivate;
class SystemNetworkInfo : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(SystemNetworkInfo)

 public:
    explicit SystemNetworkInfo(QObject *parent = 0);

    virtual QNetworkInfo::NetworkMode currentNetworkMode();
    QNetworkAccessManager::NetworkAccessibility networkAccessible();

 signals:

    // fwd QNetworkInfo signals
    void cellIdChanged(int interface, const QString& id);
    void currentCellDataTechnologyChanged(int interface, QNetworkInfo::CellDataTechnology tech);
    void currentMobileCountryCodeChanged(int interface, const QString& mcc);
    void currentMobileNetworkCodeChanged(int interface, const QString& mnc);
    void currentNetworkModeChanged(QNetworkInfo::NetworkMode mode);
    void locationAreaCodeChanged(int interface, const QString& lac);
    void networkInterfaceCountChanged(QNetworkInfo::NetworkMode mode, int count);
    void networkNameChanged(QNetworkInfo::NetworkMode mode, int interface, const QString& name);
    void networkSignalStrengthChanged(QNetworkInfo::NetworkMode mode, int interface, int strength);
    void networkStatusChanged(QNetworkInfo::NetworkMode mode, int interface, QNetworkInfo::NetworkStatus status);

    // network status signals
    void networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible);

 private:
    Q_PRIVATE_SLOT(d_func(), void onOnlineStateChanged(bool))

#if DEBUG
    // track the changes so that we get some debug info, else we will just fwd the signals
    Q_PRIVATE_SLOT(d_func(),
        void onCellIdChanged(int, const QString&))
    Q_PRIVATE_SLOT(d_func(),
        void onCurrentCellDataTechnologyChanged(int,
                                              QNetworkInfo::CellDataTechnology))
    Q_PRIVATE_SLOT(d_func(),
        void onCurrentMobileCountryCodeChanged(int, const QString&))
    Q_PRIVATE_SLOT(d_func(),
        void onCurrentMobileNetworkCodeChanged(int, const QString&))
    Q_PRIVATE_SLOT(d_func(),
        void onCurrentNetworkModeChanged(QNetworkInfo::NetworkMode))
    Q_PRIVATE_SLOT(d_func(),
        void onLocationAreaCodeChanged(int, const QString&))
    Q_PRIVATE_SLOT(d_func(),
        void onNetworkInterfaceCountChanged(QNetworkInfo::NetworkMode, int))
    Q_PRIVATE_SLOT(d_func(),
        void onNetworkNameChanged(QNetworkInfo::NetworkMode, int, const QString&))
    Q_PRIVATE_SLOT(d_func(),
        void onNetworkSignalStrengthChanged(QNetworkInfo::NetworkMode, int, int))
    Q_PRIVATE_SLOT(d_func(),
        void onNetworkStatusChanged(QNetworkInfo::NetworkMode, int,
                                  QNetworkInfo::NetworkStatus))
#endif

 private:
    SystemNetworkInfoPrivate* d_ptr;
};

#endif  // DOWNLOADER_LIB_SYSTEM_NETWORK_INFO_H
