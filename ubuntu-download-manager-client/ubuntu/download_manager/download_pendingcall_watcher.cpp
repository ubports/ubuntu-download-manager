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

#include <QDebug>
#include <QDBusPendingReply>
#include "error.h"
#include "download_pendingcall_watcher.h"

namespace Ubuntu {

namespace DownloadManager {

DownloadPendingCallWatcher::DownloadPendingCallWatcher(
                                               const QDBusConnection& conn,
                                               const QString& servicePath,
                                               const QDBusPendingCall& call,
                                               Download* down,
                                               QObject* parent)
    : PendingCallWatcher(conn, servicePath, call, parent),
      _down(down) {
    connect(this, &DownloadPendingCallWatcher::finished,
        this, &DownloadPendingCallWatcher::onFinished);
}

void
DownloadPendingCallWatcher::onFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<> reply = *watcher;
    if (reply.isError()) {
        qDebug() << "ERROR" << reply.error() << reply.error().type();
        auto err = new Error(reply.error());
        _down->error(err);
    }
    watcher->deleteLater();
}

}  // DownloadManager

}  // Ubuntu
