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

#include <QDebug>
#include <QDBusPendingReply>
#include <QDBusObjectPath>
#include "download_impl.h"
#include "downloads_list_impl.h"
#include "error.h"
#include "group_download.h"
#include "manager.h"
#include "manager_pendingcall_watcher.h"

namespace Ubuntu {

namespace DownloadManager {

DownloadManagerPCW::DownloadManagerPCW(const QDBusConnection& conn,
                                       const QString& servicePath,
                                       const QDBusPendingCall& call,
                                       DownloadCb cb,
                                       DownloadCb errCb,
                                       QObject* parent)
    : PendingCallWatcher(conn, servicePath, call, parent),
      _cb(cb),
      _errCb(errCb) {
    connect(this, &QDBusPendingCallWatcher::finished,
        this, &DownloadManagerPCW::onFinished);
}

void
DownloadManagerPCW::onFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<QDBusObjectPath> reply = *watcher;
    auto man = static_cast<Manager*>(parent());
    if (reply.isError()) {
        qDebug() << "ERROR" << reply.error() << reply.error().type();
        // create error and deal with it
        auto err = new DBusError(reply.error());
        auto down = new DownloadImpl(_conn, err);
        _errCb(down);
        emit man->downloadCreated(down);
    } else {
        qDebug() << "Success!";
        auto path = reply.value();
        auto down = new DownloadImpl(_conn, _servicePath, path);
        emit man->downloadCreated(down);
        _cb(down);
    }
    emit callbackExecuted();
    watcher->deleteLater();
}

DownloadsListManagerPCW::DownloadsListManagerPCW(const QDBusConnection& conn,
                                                 const QString& servicePath,
                                                 const QDBusPendingCall& call,
                                                 DownloadsListCb cb,
                                                 DownloadsListCb errCb,
                                                 QObject* parent)
    : PendingCallWatcher(conn, servicePath, call, parent),
      _cb(cb),
      _errCb(errCb) {
    connect(this, &QDBusPendingCallWatcher::finished,
        this, &DownloadsListManagerPCW::onFinished);
}

void
DownloadsListManagerPCW::onFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<QList<QDBusObjectPath> > reply = *watcher;
    DownloadsListImpl* list;
    auto man = static_cast<Manager*>(parent());
    if (reply.isError()) {
        qDebug() << "ERROR" << reply.error() << reply.error().type();
        // create error and deal with it
        auto err = new DBusError(reply.error());
        list = new DownloadsListImpl(err);
        _errCb(list);
        emit man->downloadsFound(list);
    } else {
        qDebug() << "Success!";
        auto paths = reply.value();
        QList<QSharedPointer<Download> > downloads;
        list = new DownloadsListImpl();
        foreach(const QDBusObjectPath& path, paths) {
            QSharedPointer<Download> down =
                QSharedPointer<Download>(new DownloadImpl(_conn,
                            _servicePath, path));
            downloads.append(down);
        }
        list = new DownloadsListImpl(downloads);
        emit man->downloadsFound(list);
        _cb(list);
    }
    emit callbackExecuted();
    watcher->deleteLater();
}

MetadataDownloadsListManagerPCW::MetadataDownloadsListManagerPCW(
                                    const QDBusConnection& conn,
                                    const QString& servicePath,
                                    const QDBusPendingCall& call,
                                    const QString& key,
                                    const QString& value,
                                    MetadataDownloadsListCb cb,
                                    MetadataDownloadsListCb errCb,
                                    QObject* parent)
    : PendingCallWatcher(conn, servicePath, call, parent),
      _key(key),
      _value(value),
      _cb(cb),
      _errCb(errCb) {
    connect(this, &QDBusPendingCallWatcher::finished,
        this, &MetadataDownloadsListManagerPCW::onFinished);
}

void
MetadataDownloadsListManagerPCW::onFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<QList<QDBusObjectPath> > reply = *watcher;
    DownloadsListImpl* list;
    auto man = static_cast<Manager*>(parent());
    if (reply.isError()) {
        qDebug() << "ERROR" << reply.error() << reply.error().type();
        // create error and deal with it
        auto err = new DBusError(reply.error());
        list = new DownloadsListImpl(err);
        _errCb(_key, _value, list);
        emit man->downloadsWithMetadataFound(_key, _value, list);
    } else {
        qDebug() << "Success!";
        auto paths = reply.value();
        QList<QSharedPointer<Download> > downloads;
        list = new DownloadsListImpl();
        foreach(const QDBusObjectPath& path, paths) {
            QSharedPointer<Download> down =
                QSharedPointer<Download>(new DownloadImpl(
                            _conn, _servicePath, path));
            downloads.append(down);
        }
        list = new DownloadsListImpl(downloads);
        emit man->downloadsWithMetadataFound(_key, _value, list);
        _cb(_key, _value, list);
    }
    emit callbackExecuted();
    watcher->deleteLater();
}

GroupManagerPCW::GroupManagerPCW(const QDBusConnection& conn,
                                 const QString& servicePath,
                                 const QDBusPendingCall& call,
                                 GroupCb cb,
                                 GroupCb errCb,
                                 QObject* parent)
    : PendingCallWatcher(conn, servicePath, call, parent),
      _cb(cb),
      _errCb(errCb) {
    connect(this, &QDBusPendingCallWatcher::finished,
        this, &GroupManagerPCW::onFinished);
}

void
GroupManagerPCW::onFinished(QDBusPendingCallWatcher* watcher) {
    QDBusPendingReply<QDBusObjectPath> reply = *watcher;
    auto man = static_cast<Manager*>(parent());
    if (reply.isError()) {
        // creater error and deal with it
        auto err = new DBusError(reply.error());
        auto down = new GroupDownload(err);
        _errCb(down);
        emit man->groupCreated(down);
    } else {
        QDBusObjectPath path = reply.value();
        auto down = new GroupDownload(path);
        emit man->groupCreated(down);
        _cb(down);
    }
    emit callbackExecuted();
    watcher->deleteLater();
}

}  // DownloadManager

}  // Ubuntu
