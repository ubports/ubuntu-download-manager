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

#include <QDBusPendingReply>
#include <QDBusObjectPath>
#include "download.h"
#include "error.h"
#include "group_download.h"
#include "manager_pendingcall_watcher.h"

namespace Ubuntu {

namespace DownloadManager {


DownloadManagerPendingCallWatcher::DownloadManagerPendingCallWatcher(
                                                  const QDBusPendingCall& call,
                                                  DownloadCreationCb cb,
                                                  ErrorCb errCb,
                                                  QObject* parent)
    : DBus::PendingCallWatcher(call, parent),
      _cb(cb),
      _errCb(errCb) {
    connect(this, &PendingCallWatcher::finished,
        this, &DownloadManagerPendingCallWatcher::onFinished);
}

void DownloadManagerPendingCallWatcher::onFinished(
                                          DBus::PendingReply* reply) {
        if (reply->isError()) {
            // creater error and deal with it
            Error* err = new Error(reply->error());
            _errCb(err);
        } else {
            QDBusObjectPath path = reply->value<QDBusObjectPath>();
            Download* down = new Download(path);
            _cb(down);
        }
        reply->deleteLater();
    emit callbackExecuted();
}


GroupManagerPendingCallWatcher::GroupManagerPendingCallWatcher(
                                            const QDBusPendingCall& call,
                                            GroupCreationCb cb,
                                            ErrorCb errCb,
                                            QObject* parent)
    : DBus::PendingCallWatcher(call, parent),
      _cb(cb),
      _errCb(errCb) {
    connect(this, &PendingCallWatcher::finished,
        this, &GroupManagerPendingCallWatcher::onFinished);
}

void GroupManagerPendingCallWatcher::onFinished(DBus::PendingReply* reply) {
        if (reply->isError()) {
            // creater error and deal with it
            Error* err = new Error(reply->error());
            _errCb(err);
        } else {
            QDBusObjectPath path = reply->value<QDBusObjectPath>();
            GroupDownload* down = new GroupDownload(path);
            _cb(down);
        }
        reply->deleteLater();
    emit callbackExecuted();
}

}  // DownloadManager

}  // Ubuntu
