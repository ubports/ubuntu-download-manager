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

#ifndef APP_DOWNLOADER_LIB_DOWNLOADER_H
#define APP_DOWNLOADER_LIB_DOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QtDBus/QDBusObjectPath>
#include "dbus_connection.h"
#include "app_download.h"

class DownloaderPrivate;
class Downloader : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Downloader)
public:
    explicit Downloader(DBusConnection* connection, QObject *parent = 0);
    void loadPreviewsDownloads(const QString &path);

public slots:
    QDBusObjectPath createDownload(const QString &id, const QString &name, const QString &url);
    QDBusObjectPath createDownloadWithHash(const QString &id, const QString &name, const QString &url,
        const QString &algorithm, const QString &hash);
    QList<QDBusObjectPath> getAllDownloads();

Q_SIGNALS:
    void downloadCreated(const QDBusObjectPath &path);

private:
    Q_PRIVATE_SLOT(d_func(), void onDownloadRemoved(QString))

private:
    // use pimpl so that we can mantains ABI compatibility
    DownloaderPrivate* d_ptr;
};

#endif // DOWNLOADER_H
