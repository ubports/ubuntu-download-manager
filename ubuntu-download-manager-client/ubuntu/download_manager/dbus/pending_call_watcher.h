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


#ifndef UBUNTU_DOWNLOADMANAGER_CLIENT_PENDING_CALL_WATCHER_H
#define UBUNTU_DOWNLOADMANAGER_CLIENT_PENDING_CALL_WATCHER_H

#include <QDBusPendingCallWatcher>
#include <QDBusPendingCall>
#include <QObject>
#include "pending_reply.h"

namespace Ubuntu {

namespace DownloadManager {

namespace DBus {

class PendingCallWatcher : public QObject {
    Q_OBJECT

 public:
    PendingCallWatcher(const QDBusPendingCall& call, QObject* parent = 0);
    
 signals:
   void finished(PendingReply* call);

 private slots:
    void onFinished(QDBusPendingCallWatcher* watch);

 private:
    QDBusPendingCallWatcher* _watch;
    
};

}  // DBus

}  // DownloadManager

}  // Ubuntu

#endif // UBUNTU_DOWNLOADMANAGER_CLIENT_PENDING_CALL_WATCHER_H
