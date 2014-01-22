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

#ifndef DOWNLOADER_LIB_DOWNLOAD_DAEMON_H
#define DOWNLOADER_LIB_DOWNLOAD_DAEMON_H

#include <QObject>
#include <QSslCertificate>
#include <QSharedPointer>
#include <ubuntu/download_manager/system/dbus_connection.h>
#include "app-downloader-lib_global.h"

namespace Ubuntu {

namespace DownloadManager {

namespace System {

class Application;
class DBusConnection;
class Timer;

}

namespace Daemon {

class Manager;
class DaemonPrivate;
class APPDOWNLOADERLIBSHARED_EXPORT Daemon : public QObject {
    Q_DECLARE_PRIVATE(Daemon)
    Q_OBJECT

 public:
    explicit Daemon(QObject *parent = 0);
    Daemon(System::Application* app,
           System::DBusConnection* conn,
           System::Timer* timer,
           Manager* man,
           QObject *parent = 0);
    virtual ~Daemon();

    bool isTimeoutEnabled();
    void enableTimeout(bool enabled);

    bool isStoppable();
    void setStoppable(bool stoppable);

    QList<QSslCertificate> selfSignedCerts();
    void setSelfSignedCerts(QList<QSslCertificate> cert);

 public slots:  // NOLINT (whitespace/indent)
    void start(QString path="com.canonical.applications.Downloader");
    void stop();

 private:
    Q_PRIVATE_SLOT(d_func(), void onTimeout())
    Q_PRIVATE_SLOT(d_func(), void onDownloadManagerSizeChanged(int))  // NOLINT (readability/function)

 private:
    // use pimpl so that we can mantains ABI compatibility
    DaemonPrivate* d_ptr;
};

}  // Daemon

}  // DownloadManager

}  // Ubuntu
#endif  // DOWNLOADER_LIB_DOWNLOAD_DAEMON_H
