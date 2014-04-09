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
#include <glog/logging.h>
#include "error.h"
#include "download_pendingcall_watcher.h"

namespace Ubuntu {

namespace DownloadManager {

DownloadPCW::DownloadPCW(
                                               const QDBusConnection& conn,
                                               const QString& servicePath,
                                               const QDBusPendingCall& call,
                                               Download* parent)
    : PendingCallWatcher(conn, servicePath, call, parent) {
    CHECK(connect(this, &DownloadPCW::finished,
        this, &DownloadPCW::onFinished))
            << "Could not connect to signal";
}

void
DownloadPCW::onFinished(QDBusPendingCallWatcher* watcher) {
    auto down = qobject_cast<Download*>(parent());
    QDBusPendingReply<> reply = *watcher;
    if (reply.isError()) {
        qDebug() << "ERROR" << reply.error() << reply.error().type();
        auto err = new DBusError(reply.error());
        down->error(err);
    }
    watcher->deleteLater();
}

}  // DownloadManager

}  // Ubuntu
