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

#include <QDebug>
#include <QSignalMapper>
#include "downloads/queue.h"
#include "system/logger.h"
#include "system/system_network_info.h"

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

Queue::Queue(QObject* parent)
    : QObject(parent),
      _current("") {
    connect(SystemNetworkInfo::instance(),
        &SystemNetworkInfo::currentNetworkModeChanged,
        this, &Queue::onCurrentNetworkModeChanged);
}

void
Queue::add(Download* download) {
    // connect to the signals and append to the list
    QString path = download->path();
    TRACE << path;

    _sortedPaths.append(path);
    _downloads[path] = download;

    connect(download, &Download::stateChanged,
        this, &Queue::onDownloadStateChanged);
    emit downloadAdded(path);
}

void
Queue::remove(const QString& path) {
    TRACE << path;

    Download* down = _downloads[path];
    _sortedPaths.removeOne(path);
    _downloads.remove(path);

    down->deleteLater();
    emit downloadRemoved(path);
}

QString
Queue::currentDownload() {
    return _current;
}

QStringList
Queue::paths() {
    return _sortedPaths;
}

QHash<QString, Download*>
Queue::downloads() {
    QHash<QString, Download*> downloads;
    foreach(const QString& path, _sortedPaths) {
        downloads[path] = _downloads[path];
    }
    return downloads;
}

int
Queue::size() {
    return _downloads.size();
}

void
Queue::onDownloadStateChanged() {
    TRACE;
    // get the appdownload that emited the signal and
    // decide what to do with it
    Download* down = qobject_cast<Download*>(sender());
    switch (down->state()) {
        case Download::START:
            // only start the download in the update method
            if (_current.isEmpty())
                updateCurrentDownload();
            break;
        case Download::PAUSE:
            down->pauseDownload();
            if (!_current.isEmpty()  && _current == down->path())
                updateCurrentDownload();
            break;
        case Download::RESUME:
            // only resume the download in the update method
            if (_current.isEmpty())
                updateCurrentDownload();
            break;
        case Download::CANCEL:
            // cancel and remove the download
            down->cancelDownload();
            if (!_current.isEmpty() && _current == down->path())
                updateCurrentDownload();
            else
                remove(down->path());
            break;
        case Download::ERROR:
        case Download::FINISH:
            // remove the registered object in dbus, remove the download
            // and the adapter from the list
            if (!_current.isEmpty() && _current == down->path())
                updateCurrentDownload();
            break;
        default:
            // do nothing
            break;
    }
}

void
Queue::onCurrentNetworkModeChanged(QNetworkInfo::NetworkMode mode) {
    TRACE << mode;
    if (mode != QNetworkInfo::UnknownMode) {
        updateCurrentDownload();
    }
}

void
Queue::updateCurrentDownload() {
    TRACE;
    if (!_current.isEmpty()) {
        // check if it was canceled/finished
        Download* currentDownload = _downloads[_current];
        Download::State state = currentDownload->state();
        if (state == Download::CANCEL || state == Download::FINISH
            || state == Download::ERROR) {
            qDebug() << "State is CANCEL || FINISH || ERROR";
            remove(_current);
            _current = "";
        } else if (!currentDownload->canDownload()
                || state == Download::PAUSE) {
            qDebug() << "States is Cannot Download || PAUSE";
            _current = "";
        } else {
            return;
        }
    }

    // loop via the downloads and choose the first that is
    // started or resumed
    foreach(const QString& path, _sortedPaths) {
        Download* down = _downloads[path];
        Download::State state = down->state();
        if (down->canDownload()
                && (state == Download::START
                    || state == Download::RESUME)) {
            _current = path;
            if (state == Download::START)
                down->startDownload();
            else
                down->resumeDownload();
            break;
        }
    }

    emit currentChanged(_current);
}

}  // Daemon

}  // DownloadManager

}  // Ubuntu
