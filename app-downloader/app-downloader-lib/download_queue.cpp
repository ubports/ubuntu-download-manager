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

typedef QHash<QString, QPair<Download*, ApplicationDownloadAdaptor*> > DownloadList;
class DownloadQueuePrivate
{
    Q_DECLARE_PUBLIC(DownloadQueue)
public:
    explicit DownloadQueuePrivate(DownloadQueue* parent);

    void add(Download* download, ApplicationDownloadAdaptor* adaptor);
    void add(const QPair<Download*, ApplicationDownloadAdaptor*>& value);
    void remove(const QString& path);

    QString currentDownload();
    QStringList paths();

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

void DownloadQueuePrivate::add(Download* download, ApplicationDownloadAdaptor* adaptor)
{
    QPair<Download*, ApplicationDownloadAdaptor*> pair(download, adaptor);
    add(pair);
}

void DownloadQueuePrivate::add(const QPair<Download*, ApplicationDownloadAdaptor*>& value)
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

void DownloadQueuePrivate::remove(const QString& path)
{
    Q_Q(DownloadQueue);
    QPair<Download*, ApplicationDownloadAdaptor*> pair = _downloads[path];
    pair.first->deleteLater();
    pair.second->deleteLater();
    _sortedPaths.removeOne(path);
    _downloads.remove(path);

    // emit the signal that it was removed
    emit q->downloadRemoved(path);
}

QString DownloadQueuePrivate::currentDownload()
{
    return _current;
}

QStringList DownloadQueuePrivate::paths()
{
    return _sortedPaths;
}

void DownloadQueuePrivate::onDownloadStateChanged()
{
    Q_Q(DownloadQueue);
    // get the appdownload that emited the signal and decide what to do with it
    Download* sender = qobject_cast<Download*>(q->sender());
    switch(sender->state())
    {
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
            if(_current.isEmpty())
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
        Download* currentDownload = _downloads[_current].first;
        Download::State state = currentDownload->state();
        if (state == Download::CANCELED || state == Download::FINISHED)
        {
            remove(_current);
            _current = "";
        }
        else
            // it was paused
            _current = "";
    }

    // loop via the downloads and choose the first that is started or resumed
    foreach(const QString& path, _sortedPaths)
    {
        QPair<Download*, ApplicationDownloadAdaptor*> pair = _downloads[path];
        Download::State state = pair.first->state();
        if(state == Download::STARTED || state == Download::RESUMED)
        {
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

DownloadQueue::DownloadQueue(QObject* parent) :
    QObject(parent),
    d_ptr(new DownloadQueuePrivate(this))
{
}

void DownloadQueue::add(Download* download, ApplicationDownloadAdaptor* adaptor)
{
    Q_D(DownloadQueue);
    d->add(download, adaptor);
}

void DownloadQueue::add(const QPair<Download*, ApplicationDownloadAdaptor*>& value)
{
    Q_D(DownloadQueue);
    d->add(value);
}

QString DownloadQueue::currentDownload()
{
    Q_D(DownloadQueue);
    return d->currentDownload();
}

QStringList DownloadQueue::paths()
{
    Q_D(DownloadQueue);
    return d->paths();
}


#include "moc_download_queue.cpp"
