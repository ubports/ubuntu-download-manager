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

#include <QtDBus/QDBusConnection>
#include <QDebug>
#include <QSharedPointer>
#include "./logger.h"
#include "./download_manager.h"
#include "./download_manager_adaptor.h"
#include "./download_daemon.h"

/**
 * PRIVATE IMPLEMENATION
 */

class DownloadDaemonPrivate {
    Q_DECLARE_PUBLIC(DownloadDaemon)

 public:
    explicit DownloadDaemonPrivate(DownloadDaemon* parent);
    explicit DownloadDaemonPrivate(DBusConnection* conn,
                                   DownloadDaemon* parent);
    ~DownloadDaemonPrivate();

    bool start();

 private:
    void init();

 private:
    QSharedPointer<DBusConnection> _conn;
    DownloadManager* _downInterface;
    DownloadManagerAdaptor* _downAdaptor;
    DownloadDaemon* q_ptr;
};

DownloadDaemonPrivate::DownloadDaemonPrivate(DownloadDaemon* parent)
    : q_ptr(parent) {
    _conn = QSharedPointer<DBusConnection>(new DBusConnection());
    _downInterface = new DownloadManager(_conn, q_ptr);
    init();
}

DownloadDaemonPrivate::DownloadDaemonPrivate(DBusConnection* conn,
                                             DownloadDaemon* parent)
    : _conn(conn),
      q_ptr(parent) {
      _downInterface = new DownloadManager(_conn);
    init();
}

void DownloadDaemonPrivate::init() {
    // set logging
    Logger::setupLogging();
#ifdef QT_DEBUG
    Logger::setLogLevel(QtDebugMsg);
#else
    if (qgetenv("UBUNTU_DOWNLOADER_DEBUG") != "")
        Logger::setLogLevel(QtDebugMsg);
#endif
}

DownloadDaemonPrivate::~DownloadDaemonPrivate() {
    // no need to delete the adaptor because the interface is its parent
    if (_downInterface)
        delete _downInterface;

    // stop logging
    Logger::setupLogging();
}

bool
DownloadDaemonPrivate::start() {
    qDebug() << "Starting daemon";
    _downAdaptor = new DownloadManagerAdaptor(_downInterface);
    bool ret = _conn->registerService("com.canonical.applications.Downloader");
    if (ret) {
        qDebug() << "Service registered to"
            << "com.canonical.applications.Downloader";
        ret = _conn->registerObject("/", _downInterface);
        qDebug() << ret;
        return ret;
    }
    qDebug() << "Could not register";
    return false;
}

/**
 * PUBLIC IMPLEMENTATION
 */

DownloadDaemon::DownloadDaemon(QObject *parent)
    : QObject(parent),
      d_ptr(new DownloadDaemonPrivate(this)) {
}

DownloadDaemon::DownloadDaemon(DBusConnection* conn, QObject *parent)
    : QObject(parent),
      d_ptr(new DownloadDaemonPrivate(conn, this)) {
}

bool
DownloadDaemon::start() {
    Q_D(DownloadDaemon);
    return d->start();
}
