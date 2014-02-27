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

#ifndef UBUNTU_GENERAL_LIB_TRANSFER_H
#define UBUNTU_GENERAL_LIB_TRANSFER_H

#include <QObject>
#include "ubuntu/transfers/system/system_network_info.h"

namespace Ubuntu {

namespace Transfers {

class Transfer : public QObject {
    Q_OBJECT

 public:
    enum State {
        IDLE,
        START,
        PAUSE,
        RESUME,
        CANCEL,
        FINISH,
        ERROR
    };

    Transfer(const QString& id,
             const QString& path,
             bool isConfined,
             QObject* parent = 0);

    QString transferId() const;
    QString path() const;
    bool isConfined() const;
    virtual Transfer::State state() const;
    void setState(Transfer::State state);
    virtual bool canTransfer();
    virtual bool isValid() const;
    bool addToQueue() const;

    // methods to be overridden by the children
    virtual bool pausable() = 0;
    virtual void cancelTransfer() = 0;
    virtual void pauseTransfer() = 0;
    virtual void resumeTransfer() = 0;
    virtual void startTransfer() = 0;

 public slots:  // NOLINT(whitespace/indent)

    virtual void setThrottle(qulonglong speed);
    virtual qulonglong throttle();
    virtual void allowGSMDownload(bool allowed);
    virtual bool isGSMDownloadAllowed();

    virtual void cancel();
    virtual void pause();
    virtual void resume();
    virtual void start();

 signals:
    void canceled(bool success);
    void error(const QString& error);
    void paused(bool success);
    void resumed(bool success);
    void started(bool success);

    // internal signals
    void stateChanged();
    void throttleChanged();

 private:
    bool _isValid = true;
    bool _addToQueue = true;
    QString _lastError = "";
    QString _id;
    qulonglong _throttle;
    bool _allowMobileData;
    Transfer::State _state;
    QString _dbusPath;
    bool _isConfined;
    System::SystemNetworkInfo* _networkInfo;
};

}  // Transfers

}  // Ubuntu

#endif
