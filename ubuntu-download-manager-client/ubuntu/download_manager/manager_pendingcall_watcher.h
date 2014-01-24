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

#ifndef UBUNTU_DOWNLOADMANAGER_CLIENT_MANAGER_PENDINGCALL_WATCHER_H
#define UBUNTU_DOWNLOADMANAGER_CLIENT_MANAGER_PENDINGCALL_WATCHER_H

#include <functional>
#include <QDBusPendingCallWatcher>

namespace Ubuntu {

namespace DownloadManager {

class Download;
class Error;
class GroupDownload;

typedef std::function<void(Download*)> DownloadCb;
typedef std::function<void(GroupDownload*)> GroupCb;

class DownloadManagerPendingCallWatcher : public QDBusPendingCallWatcher {
    Q_OBJECT

 public:
    DownloadManagerPendingCallWatcher(const QDBusPendingCall& call,
                                      DownloadCb cb,
                                      DownloadCb errCb,
                                      QObject* parent = 0);
    
 signals:
    void callbackExecuted();
    
 private slots:
    void onFinished(QDBusPendingCallWatcher* watcher);

 private:
    DownloadCb _cb;
    DownloadCb _errCb;
};


class GroupManagerPendingCallWatcher : public QDBusPendingCallWatcher {
    Q_OBJECT

 public:
    GroupManagerPendingCallWatcher(const QDBusPendingCall& call,
                                   GroupCb cb,
                                   GroupCb errCb,
                                   QObject* parent = 0);

 signals:
    void callbackExecuted();

 private slots:
    void onFinished(QDBusPendingCallWatcher* watcher);

 private:
    GroupCb _cb;
    GroupCb _errCb;
};

}  // DownloadManager

}  // Ubuntu

#endif  // UBUNTU_DOWNLOADMANAGER_CLIENT_MANAGER_PENDINGCALL_WATCHER_H
