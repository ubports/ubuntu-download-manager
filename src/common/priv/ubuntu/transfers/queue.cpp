/*
 * Copyright 2013-2014 Canonical Ltd.
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

#include <QSignalMapper>
#include <glog/logging.h>

#include "ubuntu/transfers/system/logger.h"
#include "ubuntu/transfers/system/network_session.h"
#include "queue.h"

namespace Ubuntu {

namespace Transfers {

Queue::Queue(QObject* parent)
    : QObject(parent) {
    CHECK(connect(NetworkSession::instance(),
        &NetworkSession::sessionTypeChanged,
        this, &Queue::onSessionTypeChanged))
            << "Could not connect to signal";
}

void
Queue::add(Transfer* transfer) {
    // connect to the signals and append to the list
    auto path = transfer->path();
    TRACE << path;

    if (!_sortedPaths.contains(transfer->transferAppId())) {
        _sortedPaths[transfer->transferAppId()] = new QStringList();
    }
    _sortedPaths[transfer->transferAppId()]->append(path);
    _transfers[path] = transfer;

    if (transfer->addToQueue()) {
        CHECK(connect(transfer, &Transfer::stateChanged,
            this, &Queue::onManagedTransferStateChanged))
                << "Could not connect to signal";
    } else {
        CHECK(connect(transfer, &Transfer::stateChanged,
            this, &Queue::onUnmanagedTransferStateChanged))
                << "Could not connect to signal";
    }

    emit transferAdded(path);
}

void
Queue::remove(const QString& path) {
    TRACE << path;

    auto transfer = _transfers[path];
    _sortedPaths[transfer->transferAppId()]->removeOne(path);
    _transfers.remove(path);

    transfer->deleteLater();
    emit transferRemoved(path);
}

QString
Queue::currentTransfer(const QString& appId) {
    if(_current.contains(appId)) {
        return _current[appId];
    } else {
        return "";
    }
}

QStringList
Queue::paths() {
    QStringList allPaths;
    foreach(QStringList* list, _sortedPaths.values()) {
        allPaths += *list;
    }
    return allPaths;
}

QHash<QString, Transfer*>
Queue::transfers() {
    QHash<QString, Transfer*> transfers;
    foreach(const QString& path, paths()) {
        transfers[path] = _transfers[path];
    }
    return transfers;
}

int
Queue::size() {
    return _transfers.size();
}

void
Queue::onManagedTransferStateChanged() {
    TRACE;
    // get the appdownload that emited the signal and
    // decide what to do with it
    auto transfer = qobject_cast<Transfer*>(sender());
    switch (transfer->state()) {
        case Transfer::RESUME:
        case Transfer::START:
            if (!_current.contains(transfer->transferAppId()) 
                    || (_current.contains(transfer->transferAppId()) 
                        && _current[transfer->transferAppId()].isEmpty())) {
                // only start or resume the transfer in the update method
                updateCurrentTransfer(transfer->transferAppId());
            }
            break;
        case Transfer::PAUSE:
            transfer->pauseTransfer();
            updateCurrentTransfer(transfer->transferAppId());
            break;
        case Transfer::CANCEL:
            // cancel and remove the transfer
            transfer->cancelTransfer();
            if (_current.contains(transfer->transferAppId()) && _current[transfer->transferAppId()] == transfer->path())
                updateCurrentTransfer(transfer->transferAppId());
            else
                remove(transfer->path());
            break;
        case Transfer::ERROR:
        case Transfer::UNCOLLECTED:
            // remove the registered object in dbus, remove the transfer
            // and the adapter from the list
            if (_current.contains(transfer->transferAppId()) && _current[transfer->transferAppId()] == transfer->path())
                updateCurrentTransfer(transfer->transferAppId());
            break;
        case Transfer::FINISH:
            if (_current.contains(transfer->transferAppId()) && _current[transfer->transferAppId()] == transfer->path()) {
                updateCurrentTransfer(transfer->transferAppId());
            } else {
                // Remove from the queue even if it wasn't the current transfer
                // (finished signals can be received for downloads that completed
                // previously but were left in an uncollected state)
                remove(transfer->path());
            }
            break;
        default:
            // do nothing
            break;
    }
}

void
Queue::onUnmanagedTransferStateChanged() {
    TRACE;
    // grab the transfer and clean it when needed
    auto transfer = qobject_cast<Transfer*>(sender());
    switch (transfer->state()) {
        case Transfer::CANCEL:
        case Transfer::ERROR:
        case Transfer::FINISH:
            // remove the registered object in dbus, remove the transfer
            // and the adapter from the list
            remove(transfer->path());
            break;
        default:
            // do nothing
            break;
    }
}

void
Queue::onSessionTypeChanged(QNetworkConfiguration::BearerType type) {
    TRACE << type;
    if (type != QNetworkConfiguration::BearerUnknown) {
        updateCurrentTransfer();
    }
}

void
Queue::updateCurrentTransfer(const QString& appIdToUpdate) {
    TRACE;

    // If we don't get given a specific appId to update transfers for
    // we update all the transfers
    QStringList appIds;
    if (appIdToUpdate.isEmpty()) {
        appIds = _sortedPaths.keys();
    } else {
        appIds.append(appIdToUpdate);
    }

    foreach(QString appId, appIds) {
        if (_current.contains(appId)) {
            // check if it was canceled/finished
            auto currentTransfer = _transfers[_current[appId]];
            auto state = currentTransfer->state();
            if (state == Transfer::CANCEL || state == Transfer::FINISH
                || state == Transfer::ERROR) {
                LOG(INFO) << "State is CANCEL || FINISH || ERROR";
                remove(_current[appId]);
                _current.remove(appId);
            } else if (state == Transfer::UNCOLLECTED) {
                LOG(INFO) << "State is UNCOLLECTED";
                _current.remove(appId);
            } else if (!currentTransfer->canTransfer()
                    || state == Transfer::PAUSE) {
                LOG(INFO) << "States is Cannot Transfer || PAUSE";
                _current.remove(appId);
            } else {
                break;
            }
        }

        // loop via the transfers and choose the first that is
        // started or resumed
        foreach(const QString& path, *_sortedPaths[appId]) {
            auto transfer = _transfers[path];
            auto state = transfer->state();
            if (transfer->canTransfer()
                    && (state == Transfer::START
                        || state == Transfer::RESUME)) {
                _current[appId] = path;
                if (state == Transfer::START) {
                    transfer->startTransfer();
                } else
                    transfer->resumeTransfer();
                break;
            }
        }
        if (_current.contains(appId) && !_current[appId].isEmpty()) {
            emit currentChanged(appId, _current[appId]);
        } else {
            emit currentChanged(appId, "");
        }
    }
}

}  // Transfers

}  // Ubuntu
