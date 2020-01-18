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
#include "ubuntu/transfers/system/network_session.h"

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
        UNCOLLECTED,
        FINISH,
        ERROR
    };

    Transfer(const QString& id,
             const QString& appId,
             const QString& path,
             bool isConfined,
             const QString& rootPath,
             QObject* parent = 0);

    virtual QString transferId() const;
    virtual QString transferAppId() const;
    virtual void setTransferAppId(QString appId);
    virtual QString path() const;
    virtual bool isConfined() const;
    virtual QString rootPath() const;
    virtual Transfer::State state() const;
    virtual void setState(Transfer::State state);
    virtual bool canTransfer();
    virtual bool isValid() const;
    virtual QString lastError() const;
    virtual bool addToQueue() const;
    virtual void setAddToQueue(bool addToQueue);

    // methods to be overridden by the children
    virtual bool pausable() { return false; }
    virtual void cancelTransfer() {}
    virtual void pauseTransfer() {}
    virtual void resumeTransfer() {}
    virtual void startTransfer() {}

 public slots:  // NOLINT(whitespace/indent)

    virtual void setThrottle(qulonglong speed);
    virtual qulonglong throttle();
    virtual void allowGSMData(bool allowed);
    virtual bool isGSMDataAllowed();

    virtual void cancel();
    virtual void pause();
    virtual void resume();
    virtual void start();
    virtual void collected();

 signals:
    void canceled(bool success);
    void error(const QString& error);
    void paused(bool success);
    void resumed(bool success);
    void started(bool success);

    // internal signals
    void stateChanged();
    void throttleChanged();

 protected:
    void setIsValid(bool isValid);
    void setLastError(const QString& lastError);

 private:
    bool _isValid = true;
    bool _addToQueue = true;
    QString _lastError = QString();
    QString _id = QString();
    QString _appId = QString();
    qulonglong _throttle = 0;
    bool _allowMobileData = true;
    Transfer::State _state = State::IDLE;
    QString _dbusPath = QString();
    bool _isConfined = true;
    QString _rootPath = QString();
    System::NetworkSession* _networkSession = nullptr;
};

}  // Transfers

}  // Ubuntu

#endif
