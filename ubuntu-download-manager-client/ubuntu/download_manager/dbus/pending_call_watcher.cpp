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

#include "pending_call_watcher.h"

namespace Ubuntu {

namespace DownloadManager {

namespace DBus {

PendingCallWatcher::PendingCallWatcher(const QDBusPendingCall& call,
                                       QObject* parent)
    : QObject(parent){
    // WARNING: do not set the parent and therefore a mem leak can happen
    // in the slots do not call deleteLater
    _watch = new QDBusPendingCallWatcher(call);

    connect(_watch, &QDBusPendingCallWatcher::finished,
        this, &PendingCallWatcher::onFinished);
}

void
PendingCallWatcher::onFinished(QDBusPendingCallWatcher* watch) {
    PendingReply* reply = new PendingReply(watch, this);
    emit finished(reply);
}

}  // DBus

}  // DownloadManager

}  // Ubuntu
