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
    : QObject(parent),
      _current("") {
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

    _sortedPaths.append(path);
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
    _sortedPaths.removeOne(path);
    _transfers.remove(path);

    transfer->deleteLater();
    emit transferRemoved(path);
}

QString
Queue::currentTransfer() {
    return _current;
}

QStringList
Queue::paths() {
    return _sortedPaths;
}

QHash<QString, Transfer*>
Queue::transfers() {
    QHash<QString, Transfer*> transfers;
    foreach(const QString& path, _sortedPaths) {
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
    qDebug() << "State changed" << transfer;
    switch (transfer->state()) {
        case Transfer::START:
            // only start the transfer in the update method
            if (_current.isEmpty())
                updateCurrentTransfer();
            break;
        case Transfer::PAUSE:
            transfer->pauseTransfer();
            if (!_current.isEmpty()  && _current == transfer->path())
                updateCurrentTransfer();
            break;
        case Transfer::RESUME:
            // only resume the transfer in the update method
            qDebug() << "State changed to resume.";
            if (_current.isEmpty()) {
                qDebug() << "There is no current download present.";
                updateCurrentTransfer();
            }
            break;
        case Transfer::CANCEL:
            // cancel and remove the transfer
            transfer->cancelTransfer();
            if (!_current.isEmpty() && _current == transfer->path())
                updateCurrentTransfer();
            else
                remove(transfer->path());
            break;
        case Transfer::ERROR:
        case Transfer::FINISH:
            // remove the registered object in dbus, remove the transfer
            // and the adapter from the list
            if (!_current.isEmpty() && _current == transfer->path())
                updateCurrentTransfer();
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
Queue::updateCurrentTransfer() {
    TRACE;
    if (!_current.isEmpty()) {
        // check if it was canceled/finished
        auto currentTransfer = _transfers[_current];
        auto state = currentTransfer->state();
        if (state == Transfer::CANCEL || state == Transfer::FINISH
            || state == Transfer::ERROR) {
            LOG(INFO) << "State is CANCEL || FINISH || ERROR";
            remove(_current);
            _current = "";
        } else if (!currentTransfer->canTransfer()
                || state == Transfer::PAUSE) {
            LOG(INFO) << "States is Cannot Transfer || PAUSE";
            _current = "";
        } else {
            return;
        }
    }

    // loop via the transfers and choose the first that is
    // started or resumed
    foreach(const QString& path, _sortedPaths) {
        auto transfer = _transfers[path];
        auto state = transfer->state();
        if (transfer->canTransfer()
                && (state == Transfer::START
                    || state == Transfer::RESUME)) {
            _current = path;
            qDebug() << _current;
            if (state == Transfer::START)
                transfer->startTransfer();
            else
                transfer->resumeTransfer();
            break;
        }
    }

    emit currentChanged(_current);
}

}  // Transfers

}  // Ubuntu
