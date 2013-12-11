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

#include <QDBusPendingCallWatcher>

namespace Ubuntu {

namespace DownloadManager {

class Download;
class Error;
class GroupDownload;

typedef std::function<void(Download*)> DownloadCreationCb;
typedef std::function<void(GroupDownload*)> GroupCreationCb;
typedef std::function<void(Error*)> ErrorCb;

class DownloadManagerPendingCallWatcher : public QDBusPendingCallWatcher {
    Q_OBJECT

 public:
    DownloadManagerPendingCallWatcher(const QDBusPendingCall& call,
                                      DownloadCreationCb cb,
                                      ErrorCb errCb,
                                      QObject* parent = 0);
    
 signals:
    void callbackExecuted();
    
 private slots:
    void onFinished(QDBusPendingCallWatcher* watcher);

 private:
    DownloadCreationCb _cb;
    ErrorCb _errCb;
};


class GroupManagerPendingCallWatcher : public QDBusPendingCallWatcher {
    Q_OBJECT

 public:
    GroupManagerPendingCallWatcher(const QDBusPendingCall& call,
                                   GroupCreationCb cb,
                                   ErrorCb errCb,
                                   QObject* parent = 0);

 signals:
    void callbackExecuted();

 private slots:
    void onFinished(QDBusPendingCallWatcher* watcher);

 private:
    GroupCreationCb _cb;
    ErrorCb _errCb;
};

}  // DownloadManager

}  // Ubuntu

#endif  // UBUNTU_DOWNLOADMANAGER_CLIENT_MANAGER_PENDINGCALL_WATCHER_H
