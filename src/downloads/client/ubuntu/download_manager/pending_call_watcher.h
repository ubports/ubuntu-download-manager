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

#ifndef UBUNTU_DOWNLOADMANAGER_CLIENT_PENDING_CALL_WATCHER_H
#define UBUNTU_DOWNLOADMANAGER_CLIENT_PENDING_CALL_WATCHER_H

#include <ubuntu/transfers/visibility.h>
#include <QDBusConnection>
#include <QDBusPendingCallWatcher>
#include <QObject>

namespace Ubuntu {

namespace DownloadManager {

class UBUNTU_TRANSFERS_PRIVATE PendingCallWatcher : public QDBusPendingCallWatcher {
    Q_OBJECT

 public:
    PendingCallWatcher(const QDBusConnection& conn,
                       const QString& servicePath,
                       const QDBusPendingCall& call,
                       QObject* parent = 0);

 signals:
    void callbackExecuted();

 protected:
    QDBusConnection _conn;
    QString _servicePath;
};

}  // DownloadManager

}  // Ubuntu

#endif // PENDING_CALL_WATCHER_H
