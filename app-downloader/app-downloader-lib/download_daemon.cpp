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
#include "downloader.h"
#include "downloader_adaptor.h"
#include "download_daemon.h"

/**
 * PRIVATE IMPLEMENATION
 */

class DownloadDaemonPrivate
{
    Q_DECLARE_PUBLIC(DownloadDaemon)
public:
    explicit DownloadDaemonPrivate(DownloadDaemon* parent);

    bool start();

private:
    DownloadDaemon* q_ptr;
    QDBusConnection _conn;
    Downloader* _downInterface;
    DownloaderAdaptor* _downAdaptor;

};

DownloadDaemonPrivate::DownloadDaemonPrivate(DownloadDaemon* parent):
    q_ptr(parent),
    _conn(QDBusConnection::sessionBus())
{
    _downInterface = new Downloader(_conn, q_ptr);
}

bool DownloadDaemonPrivate::start()
{
    qDebug() << "Starting daemon";
    _downAdaptor = new DownloaderAdaptor(_downInterface);
    bool ret = _conn.registerService("com.canonical.applications.Downloader");
    if (ret)
    {
        qDebug() << "Service registered to com.canonical.applications.Downloader";
        ret = _conn.registerObject("/", _downInterface);
        qDebug() << ret;
        return ret;
    }
    qDebug() << "Could not register";
    return false;
}

/**
 * PUBLIC IMPLEMENTATION
 */

DownloadDaemon::DownloadDaemon(QObject *parent) :
    QObject(parent),
    d_ptr(new DownloadDaemonPrivate(this))
{
}

bool DownloadDaemon::start()
{
    Q_D(DownloadDaemon);
    return d->start();
}
