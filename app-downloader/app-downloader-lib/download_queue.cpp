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

#include "download_queue.h"

/*
 * PRIVATE IMPLEMENTATION
 */

typedef QHash<QString, QPair<AppDownload*, ApplicationDownloadAdaptor*> > DownloadList;
class DownloadQueuePrivate
{
    Q_DECLARE_PUBLIC(DownloadQueue)
public:
    explicit DownloadQueuePrivate(DownloadQueue* parent);

    void add(AppDownload* download, ApplicationDownloadAdaptor* adaptor);
    void add(const QPair<AppDownload*, ApplicationDownloadAdaptor*>& value);

    QString currentDownload();

private:
    void onDownloadStateChanged();
    void updateCurrentDownload();

private:
    QString _current;
    DownloadList _downloads;  // quick for access
    QStringList _sortedPaths; // keep the order in witch the downloads have been added
    DownloadQueue* q_ptr;
};

DownloadQueuePrivate::DownloadQueuePrivate(DownloadQueue* parent):
    _current(""),
    q_ptr(parent)
{
}

void DownloadQueuePrivate::add(AppDownload* download, ApplicationDownloadAdaptor* adaptor)
{
    QPair<AppDownload*, ApplicationDownloadAdaptor*> pair(download, adaptor);
    add(pair);
}

void DownloadQueuePrivate::add(const QPair<AppDownload*, ApplicationDownloadAdaptor*>& value)
{
    Q_Q(DownloadQueue);
    // connect to the signals and append to the list
    QString path = value.first->path();
    _sortedPaths.append(path);
    _downloads[path] = value;

    q->connect(value.first, SIGNAL(stateChanged()),
        q, SLOT(onDownloadStateChanged()));
    emit q->downloadAdded(path);
}

QString DownloadQueuePrivate::currentDownload()
{
    return _current;
}

void DownloadQueuePrivate::onDownloadStateChanged()
{
    Q_Q(DownloadQueue);
    // get the appdownload that emited the signal and decide what to do with it
    AppDownload* sender = qobject_cast<AppDownload*>(q->sender());
    switch(sender->state())
    {
        case AppDownload::STARTED:
            // only start the download in the update method
            if (_current.isEmpty())
                updateCurrentDownload();
            break;
        case AppDownload::PAUSED:
            sender->pauseDownload();
            if (!_current.isEmpty()  && _current == sender->path())
                updateCurrentDownload();
            break;
        case AppDownload::RESUMED:
            // only resume the download in the update method
            if(_current.isEmpty())
                updateCurrentDownload();
            break;
        case AppDownload::CANCELED:
            // cancel and remove the download
            sender->cancelDownload();
            if (!_current.isEmpty() && _current == sender->path())
                updateCurrentDownload();
            break;
        case AppDownload::FINISHED:
            // remove the registered object in dbus, remove the download and the adapter from the list
            if (_current.isEmpty() && _current == sender->path())
                updateCurrentDownload();
            break;
        default:
            // do nothing
            break;
    }
}

void DownloadQueuePrivate::updateCurrentDownload()
{
    Q_Q(DownloadQueue);

    if (!_current.isEmpty())
    {
        AppDownload* currentDownload = _downloads[_current].first;
        AppDownload::State state = currentDownload->state();
        if (state == AppDownload::CANCELED || state == AppDownload::FINISHED)
        {
            QPair<AppDownload*, ApplicationDownloadAdaptor*> pair = _downloads[_current];
            pair.first->deleteLater();
            pair.second->deleteLater();
            _sortedPaths.removeOne(_current);
            _downloads.remove(_current);

            // emit the signal that it was removed
            emit q->downloadRemoved(_current);
            _current = "";
        }
        else
            // it was paused
            _current = "";
    }

    // loop via the downloads and choose the first that is started or resumed
    foreach(const QString&path, _sortedPaths)
    {
        QPair<AppDownload*, ApplicationDownloadAdaptor*> pair = _downloads[path];
        AppDownload::State state = pair.first->state();
        if(state == AppDownload::STARTED || state == AppDownload::RESUMED)
        {
            _current = path;
            if (state == AppDownload::STARTED)
                pair.first->startDownload();
            else
                pair.first->resumeDownload();

            break;
        } // is started or resumed
    }

    emit q->currentChanged(_current);
}

/*
 * PUBLIC IMPLEMENTATION
 */

DownloadQueue::DownloadQueue(QObject* parent) :
    QObject(parent),
    d_ptr(new DownloadQueuePrivate(this))
{
}

void DownloadQueue::add(AppDownload* download, ApplicationDownloadAdaptor* adaptor)
{
    Q_D(DownloadQueue);
    d->add(download, adaptor);
}

void DownloadQueue::add(const QPair<AppDownload*, ApplicationDownloadAdaptor*>& value)
{
    Q_D(DownloadQueue);
    d->add(value);
}

QString DownloadQueue::currentDownload()
{
    Q_D(DownloadQueue);
    return d->currentDownload();
}

#include "moc_download_queue.cpp"
