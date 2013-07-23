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
#include "./download_queue.h"

/*
 * PRIVATE IMPLEMENTATION
 */

typedef QHash<QString, QPair<Download*, DownloadAdaptor*> > DownloadList;
class DownloadQueuePrivate {
    Q_DECLARE_PUBLIC(DownloadQueue)

 public:
    explicit DownloadQueuePrivate(SystemNetworkInfo* networkInfo,
                                  DownloadQueue* parent);
    ~DownloadQueuePrivate();

    void add(Download* download, DownloadAdaptor* adaptor);
    void add(const QPair<Download*, DownloadAdaptor*>& value);
    void remove(const QString& path);

    QString currentDownload();
    QStringList paths();
    QHash<QString, Download*> downloads();

    void onDownloadStateChanged();
    void onDestroyed(const QString& path);
    void onCurrentNetworkModeChanged(QNetworkInfo::NetworkMode mode);

 private:
    void updateCurrentDownload();

 private:
    QSignalMapper* _mapper;
    QString _current;
    DownloadList _downloads;   // quick for access
    QStringList _sortedPaths;  // keep the order
    SystemNetworkInfo* _networkInfo;
    DownloadQueue* q_ptr;
};

DownloadQueuePrivate::DownloadQueuePrivate(SystemNetworkInfo* networkInfo,
                                           DownloadQueue* parent)
    : _current(""),
      _networkInfo(networkInfo),
      q_ptr(parent) {
    Q_Q(DownloadQueue);
    _mapper = new QSignalMapper();

    q->connect(_networkInfo,
            SIGNAL(currentNetworkModeChanged(QNetworkInfo::NetworkMode)), q,
            SLOT(onCurrentNetworkModeChanged(QNetworkInfo::NetworkMode)));

    q->connect(_mapper, SIGNAL(mapped(const QString&)),
        q, SLOT(onDestroyed(const QString&)));
}

DownloadQueuePrivate::~DownloadQueuePrivate() {
    if (_mapper != NULL)
        delete _mapper;
}

void
DownloadQueuePrivate::add(Download* download, DownloadAdaptor* adaptor) {
    QPair<Download*, DownloadAdaptor*> pair(download, adaptor);
    add(pair);
}

void
DownloadQueuePrivate::add(const QPair<Download*, DownloadAdaptor*>& value) {
    Q_Q(DownloadQueue);
    // connect to the signals and append to the list
    QString path = value.first->path();
    _sortedPaths.append(path);
    _downloads[path] = value;

    q->connect(value.first, SIGNAL(stateChanged()),
        q, SLOT(onDownloadStateChanged()));
    emit q->downloadAdded(path);
}

void
DownloadQueuePrivate::remove(const QString& path) {
    qDebug() << __FUNCTION__ << path;

    Q_Q(DownloadQueue);

    QPair<Download*, DownloadAdaptor*> pair = _downloads[path];
    _sortedPaths.removeOne(path);
    _downloads.remove(path);

    // connect to the adaptor destroyed event to ensure that we have
    // emitted all signals, else we might have a race condition where
    // the object path is removed from dbus to early
    q->connect(pair.second, SIGNAL(destroyed(QObject* obj)),
        _mapper, SLOT(map(QObject* obj)));
    _mapper->setMapping(pair.first, path);

    pair.second->deleteLater();
    pair.first->deleteLater();
}

QString
DownloadQueuePrivate::currentDownload() {
    return _current;
}

QStringList
DownloadQueuePrivate::paths() {
    return _sortedPaths;
}

QHash<QString, Download*>
DownloadQueuePrivate::downloads() {
    QHash<QString, Download*> downloads;
    foreach(const QString& path, _sortedPaths) {
        downloads[path] = _downloads[path].first;
    }
    return downloads;
}

void
DownloadQueuePrivate::onDownloadStateChanged() {
    Q_Q(DownloadQueue);
    // get the appdownload that emited the signal and decide what to do with it
    Download* sender = qobject_cast<Download*>(q->sender());
    switch (sender->state()) {
        case Download::STARTED:
            // only start the download in the update method
            if (_current.isEmpty())
                updateCurrentDownload();
            break;
        case Download::PAUSED:
            sender->pauseDownload();
            if (!_current.isEmpty()  && _current == sender->path())
                updateCurrentDownload();
            break;
        case Download::RESUMED:
            // only resume the download in the update method
            if (_current.isEmpty())
                updateCurrentDownload();
            break;
        case Download::CANCELED:
            // cancel and remove the download
            sender->cancelDownload();
            if (!_current.isEmpty() && _current == sender->path())
                updateCurrentDownload();
            else
                remove(sender->path());
            break;
        case Download::FINISHED:
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

void
DownloadQueuePrivate::onDestroyed(const QString &path) {
    qDebug() << __FUNCTION__;
    Q_Q(DownloadQueue);
    emit q->downloadRemoved(path);
}

void
DownloadQueuePrivate::onCurrentNetworkModeChanged(
        QNetworkInfo::NetworkMode mode) {
    Q_UNUSED(mode);
    updateCurrentDownload();
}

void
DownloadQueuePrivate::updateCurrentDownload() {
    Q_Q(DownloadQueue);

    if (!_current.isEmpty()) {
        // check if it was canceled/finished
        Download* currentDownload = _downloads[_current].first;
        Download::State state = currentDownload->state();
        if (state == Download::CANCELED || state == Download::FINISHED) {
            remove(_current);
            _current = "";
        } else if (!currentDownload->canDownload()
                || state == Download::PAUSED) {
            _current = "";
        } else {
            return;
        }
    }

    // loop via the downloads and choose the first that is started or resumed
    foreach(const QString& path, _sortedPaths) {
        QPair<Download*, DownloadAdaptor*> pair = _downloads[path];
        Download::State state = pair.first->state();
        if (pair.first->canDownload()
                && (state == Download::STARTED || state == Download::RESUMED)) {
            _current = path;
            if (state == Download::STARTED)
                pair.first->startDownload();
            else
                pair.first->resumeDownload();

            break;
        }
    }

    emit q->currentChanged(_current);
}

/*
 * PUBLIC IMPLEMENTATION
 */

DownloadQueue::DownloadQueue(SystemNetworkInfo* networkInfo, QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadQueuePrivate(networkInfo, this)) {
}

void
DownloadQueue::add(Download* download, DownloadAdaptor* adaptor) {
    Q_D(DownloadQueue);
    d->add(download, adaptor);
}

void
DownloadQueue::add(const QPair<Download*, DownloadAdaptor*>& value) {
    Q_D(DownloadQueue);
    d->add(value);
}

QString
DownloadQueue::currentDownload() {
    Q_D(DownloadQueue);
    return d->currentDownload();
}

QStringList
DownloadQueue::paths() {
    Q_D(DownloadQueue);
    return d->paths();
}


QHash<QString, Download*>
DownloadQueue::downloads() {
    Q_D(DownloadQueue);
    return d->downloads();
}

#include "moc_download_queue.cpp"
