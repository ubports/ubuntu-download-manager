/*
 * Copyright 2013 2013 Canonical Ltd.
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

namespace Ubuntu {

namespace DownloadManager {

/*
 * PRIVATE IMPLEMENTATION
 */

class QueuePrivate {
    Q_DECLARE_PUBLIC(Queue)

 public:
    QueuePrivate(QSharedPointer<SystemNetworkInfo> networkInfo,
                 Queue* parent)
        : _current(""),
          _networkInfo(networkInfo),
          q_ptr(parent) {
          Q_Q(Queue);

        q->connect(_networkInfo.data(),
            SIGNAL(currentNetworkModeChanged(QNetworkInfo::NetworkMode)), q,
            SLOT(onCurrentNetworkModeChanged(QNetworkInfo::NetworkMode)));
    }

    void add(Download* download) {
        Q_Q(Queue);
        // connect to the signals and append to the list
        QString path = download->path();
        TRACE << path;

        _sortedPaths.append(path);
        _downloads[path] = download;

        q->connect(download, SIGNAL(stateChanged()),
            q, SLOT(onDownloadStateChanged()));
        emit q->downloadAdded(path);
    }

    void remove(const QString& path) {
        Q_Q(Queue);
        TRACE << path;

        Download* down = _downloads[path];
        _sortedPaths.removeOne(path);
        _downloads.remove(path);

        down->deleteLater();
        emit q->downloadRemoved(path);
    }

    QString currentDownload() {
        return _current;
    }

    QStringList paths() {
        return _sortedPaths;
    }

    QHash<QString, Download*> downloads() {
        QHash<QString, Download*> downloads;
        foreach(const QString& path, _sortedPaths) {
            downloads[path] = _downloads[path];
        }
        return downloads;
    }

    int size() {
        return _downloads.size();
    }

    void onDownloadStateChanged() {
        TRACE;
        Q_Q(Queue);
        // get the appdownload that emited the signal and
        // decide what to do with it
        Download* sender = qobject_cast<Download*>(q->sender());
        switch (sender->state()) {
            case Download::START:
                // only start the download in the update method
                if (_current.isEmpty())
                    updateCurrentDownload();
                break;
            case Download::PAUSE:
                sender->pauseDownload();
                if (!_current.isEmpty()  && _current == sender->path())
                    updateCurrentDownload();
                break;
            case Download::RESUME:
                // only resume the download in the update method
                if (_current.isEmpty())
                    updateCurrentDownload();
                break;
            case Download::CANCEL:
                // cancel and remove the download
                sender->cancelDownload();
                if (!_current.isEmpty() && _current == sender->path())
                    updateCurrentDownload();
                else
                    remove(sender->path());
                break;
            case Download::ERROR:
            case Download::FINISH:
                // remove the registered object in dbus, remove the download
                // and the adapter from the list
                if (!_current.isEmpty() && _current == sender->path())
                    updateCurrentDownload();
                break;
            default:
                // do nothing
                break;
        }
    }

    void onCurrentNetworkModeChanged(QNetworkInfo::NetworkMode mode) {
        TRACE << mode;
        if (mode != QNetworkInfo::UnknownMode) {
            updateCurrentDownload();
        }
    }

 private:
    void updateCurrentDownload() {
        TRACE;
        Q_Q(Queue);

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

        emit q->currentChanged(_current);
    }

 private:
    QString _current;
    QHash<QString, Download*> _downloads;  // quick for access
    QStringList _sortedPaths;  // keep the order
    QSharedPointer<SystemNetworkInfo> _networkInfo;
    Queue* q_ptr;
};

/*
 * PUBLIC IMPLEMENTATION
 */

Queue::Queue(QSharedPointer<SystemNetworkInfo> networkInfo,
             QObject* parent)
    : QObject(parent),
      d_ptr(new QueuePrivate(networkInfo, this)) {
}

void
Queue::add(Download* download) {
    Q_D(Queue);
    d->add(download);
}

QString
Queue::currentDownload() {
    Q_D(Queue);
    return d->currentDownload();
}

QStringList
Queue::paths() {
    Q_D(Queue);
    return d->paths();
}


QHash<QString, Download*>
Queue::downloads() {
    Q_D(Queue);
    return d->downloads();
}

int
Queue::size() {
    Q_D(Queue);
    return d->size();
}

}  // DownloadManager

}  // Ubuntu

#include "moc_queue.cpp"
