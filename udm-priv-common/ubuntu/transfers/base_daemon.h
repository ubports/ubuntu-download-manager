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

#ifndef UBUNTU_GENERAL_LIB_DAEMON_H
#define UBUNTU_GENERAL_LIB_DAEMON_H

#include <functional>
#include <QObject>
#include <QSslCertificate>
#include <QSharedPointer>
#include <ubuntu/transfers/system/dbus_connection.h>

namespace Ubuntu {

namespace System {

class Application;
class DBusConnection;
class Timer;

}

namespace General {

class BaseManager;
class ManagerFactory;
class AdaptorFactory;

typedef std::function<BaseManager*(System::Application*, System::DBusConnection*)>
    ManagerConstructor;

class BaseDaemon : public QObject {
    Q_OBJECT

 public:
    BaseDaemon(ManagerFactory* managerFactory,
           AdaptorFactory* adaptorFactory,
           QObject *parent = 0);
    BaseDaemon(ManagerFactory* managerFactory,
           AdaptorFactory* adaptorFactory,
           System::Application* app,
           System::DBusConnection* conn,
           System::Timer* timer,
           QObject *parent = 0);
    virtual ~BaseDaemon();

    bool isTimeoutEnabled();
    void enableTimeout(bool enabled);

    bool isStoppable();
    void setStoppable(bool stoppable);

    QList<QSslCertificate> selfSignedCerts();
    void setSelfSignedCerts(QList<QSslCertificate> cert);

 public slots:  // NOLINT (whitespace/indent)
    virtual void start(const QString& path);
    void stop();

 protected:
    BaseManager* manager();

 private:
    void init();
    void parseCommandLine();
    void onTimeout();
    void onDownloadManagerSizeChanged(int);

 private:
    QString _path = "";
    bool _isTimeoutEnabled = true;
    bool _stoppable = false;
    QList<QSslCertificate> _certs;
    System::Application* _app = nullptr;
    System::Timer* _shutDownTimer = nullptr;
    System::DBusConnection* _conn = nullptr;
    ManagerFactory* _managerFactory = nullptr;
    AdaptorFactory* _adaptorFactory = nullptr;
    BaseManager* _manager = nullptr;
    QObject* _managerAdaptor = nullptr;
};

}  // General

}  // Ubuntu

#endif  // DOWNLOADER_LIB_DOWNLOAD_DAEMON_H
