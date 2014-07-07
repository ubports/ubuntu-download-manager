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

#include <QDBusPendingReply>

#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>

#include <ubuntu/download_manager/error.h>
#include <ubuntu/download_manager/logging/logger.h>

#include <ubuntu/download_manager/download_pendingcall_watcher.h>

namespace Ubuntu {

namespace DownloadManager {

using namespace Logging;

DownloadPCW::DownloadPCW(const QDBusConnection& conn,
                         const QString& servicePath,
                         const QDBusPendingCall& call,
                         Download* parent)
    : PendingCallWatcher(conn, servicePath, call, parent) {
    auto connected = connect(this, &DownloadPCW::finished,
        this, &DownloadPCW::onFinished);
    if (!connected) {
        Logger::log(Logger::Critical,
            "Could not connect to signal &DownloadPCW::finished");
    }
}

void
DownloadPCW::onFinished(QDBusPendingCallWatcher* watcher) {
    auto down = qobject_cast<Download*>(parent());
    QDBusPendingReply<> reply = *watcher;
    if (reply.isError()) {
        auto dbusErr = reply.error();
        Logger::log(Logger::Error,
            QString("%1 %2").arg(dbusErr.name()).arg(dbusErr.message()));
        auto err = new DBusError(reply.error());
        down->error(err);
    }
    watcher->deleteLater();
}

}  // DownloadManager

}  // Ubuntu
