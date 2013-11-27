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

#include <QAtomicPointer>
#include <QMutex>
#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>
#include <QNetworkInfo>
#include <QObject>

namespace Ubuntu {

namespace DownloadManager {

namespace System {

class SystemNetworkInfo : public QObject {
    Q_OBJECT

 public:
    virtual QNetworkInfo::NetworkMode currentNetworkMode();
    virtual bool isOnline();

    static SystemNetworkInfo* instance();

    // only used for testing so that we can inject a fake
    static void setInstance(SystemNetworkInfo* instance);

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
    void onlineStateChanged(bool online);

 protected:
    // left protected so that teh child fake class can use it
    explicit SystemNetworkInfo(QObject *parent = 0);

 private:
    void onOnlineStateChanged(bool);

#if DEBUG
    // track the changes so that we get some debug info, else we will just fwd the signals
    void onCellIdChanged(int, const QString&);
    void onCurrentCellDataTechnologyChanged(int,
        QNetworkInfo::CellDataTechnology);
    void onCurrentMobileCountryCodeChanged(int, const QString&);
    void onCurrentMobileNetworkCodeChanged(int, const QString&);
    void onCurrentNetworkModeChanged(QNetworkInfo::NetworkMode);
    void onLocationAreaCodeChanged(int, const QString&);
    void onNetworkInterfaceCountChanged(QNetworkInfo::NetworkMode, int);
    void onNetworkNameChanged(QNetworkInfo::NetworkMode, int, const QString&);
    void onNetworkSignalStrengthChanged(QNetworkInfo::NetworkMode, int, int);
    void onNetworkStatusChanged(QNetworkInfo::NetworkMode, int,
        QNetworkInfo::NetworkStatus);
#endif

 private:
    // used for the singleton
    static SystemNetworkInfo* _instance;
    static QMutex _mutex;

    // internal vars
    QNetworkInfo* _info;
    QNetworkConfigurationManager* _configMan;
};

}  // System

}  // DownloadManager

}  // Ubuntu

#endif  // DOWNLOADER_LIB_SYSTEM_NETWORK_INFO_H
