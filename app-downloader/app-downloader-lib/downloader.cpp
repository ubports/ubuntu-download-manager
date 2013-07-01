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

#include "request_factory.h"
#include "application_download_adaptor.h"
#include "downloader.h"

/**
 * PRIVATE IMPLEMENATION
 */

class DownloaderPrivate
{
    Q_DECLARE_PUBLIC(Downloader)
public:
    explicit DownloaderPrivate(DBusConnection* connection, Downloader* parent);

private:
    AppDownload* getApplication(const QString &appId, const QString &appName, const QUrl &url);
    AppDownload* getApplication(const QString &appId, const QString &appName, const QUrl &url, const QString &hash,
        QCryptographicHash::Algorithm algo);
    void addDownload(AppDownload* download);
    void updateCurrentDownload();
    void onDownloadStateChanged();
    void loadPreviewsDownloads(QString path);

    QDBusObjectPath createDownload(const QString &appId, const QString &appName, const QString &url);
    QDBusObjectPath createDownloadWithHash(const QString &appId, const QString &appName, const QString &url,
        const QString &hash, QCryptographicHash::Algorithm algo);
    QList<QDBusObjectPath> getAllDownloads();

private:
    static QString BASE_ACCOUNT_URL;

    QList<AppDownload*> _downloads;
    QHash<QString, ApplicationDownloadAdaptor*> _adaptors;
    DBusConnection* _conn;
    Downloader* q_ptr;
    RequestFactory* _reqFactory;
    AppDownload* _current;
};

QString DownloaderPrivate::BASE_ACCOUNT_URL = "/com/canonical/applications/download/%1";

DownloaderPrivate::DownloaderPrivate(DBusConnection* connection, Downloader* parent):
    _conn(connection),
    q_ptr(parent)
{
    _reqFactory = new RequestFactory();
    _current = NULL;
}

AppDownload* DownloaderPrivate::getApplication(const QString &appId, const QString &appName, const QUrl &url)
{
    Q_Q(Downloader);
    QString path = DownloaderPrivate::BASE_ACCOUNT_URL.arg(appId);
    AppDownload* appDown = new AppDownload(appId, appName, path, url, _reqFactory);
    q->connect(appDown, SIGNAL(stateChanged()),
        q, SLOT(onDownloadStateChanged()));
    return appDown;
}

AppDownload* DownloaderPrivate::getApplication(const QString &appId, const QString &appName, const QUrl &url,
    const QString &hash, QCryptographicHash::Algorithm algo)
{
    Q_Q(Downloader);
    QString path = DownloaderPrivate::BASE_ACCOUNT_URL.arg(appId);
    AppDownload* appDown = new AppDownload(appId, appName, path, url, hash, algo, _reqFactory);
    q->connect(appDown, SIGNAL(stateChanged()),
        q, SLOT(onDownloadStateChanged()));
    return appDown;
}

void DownloaderPrivate::addDownload(AppDownload* download)
{
    Q_UNUSED(download);
    // TODO
}

void DownloaderPrivate::updateCurrentDownload()
{
    qDebug() << "Updating CURRENT DOWNLOAD.";
    if (_current != NULL)
    {
        AppDownload::State state = _current->state();
        if (state == AppDownload::CANCELED || state == AppDownload::FINISHED)
        {
            qDebug() << "Current was CANCELED or FINISHED";
            // clean resources
            _conn->unregisterObject(_current->path());
            _downloads.removeOne(_current);
            ApplicationDownloadAdaptor* adaptor = _adaptors[_current->path()];
            _adaptors.remove(_current->path());
            adaptor->deleteLater();
            _current->deleteLater();
            _current = NULL;

        }
        else
        {
            qDebug() << "Current was not deleted or canceled but paused.";
            _current = NULL;
            return;
        }
    }

    qDebug() << "Updating current download.";
    // loop via the downloads and choose the first that is started or resumed
    for(int index = 0; index < _downloads.count(); index++)
    {
        AppDownload* appDownload = _downloads[index];
        AppDownload::State state = appDownload->state();
        if(state == AppDownload::STARTED || state == AppDownload::RESUMED)
        {
            qDebug() << "Found downlad ready for " << appDownload->url();
            // found the first instance that can be downloaded set it to be first
            _downloads.removeAt(index);
            _downloads.prepend(appDownload);
            _current = appDownload;

            if (state == AppDownload::STARTED)
            {
                qDebug() << "Starting download for" << appDownload->url();
                appDownload->startDownload();
            }
            else
            {
                qDebug() << "Resuming download for " << appDownload->url();
                appDownload->resumeDownload();
            }

            break;
        } // is started or resumed
    } // for
}

void DownloaderPrivate::onDownloadStateChanged()
{
    Q_Q(Downloader);
    // get the appdownload that emited the signal and decide what to do with it
    AppDownload* sender = qobject_cast<AppDownload*>(q->sender());
    switch(sender->state())
    {
        case AppDownload::STARTED:
            // only start the download in the update method
            if (_current == NULL)
                updateCurrentDownload();
            break;
        case AppDownload::PAUSED:
            sender->pauseDownload();
            if (_current != NULL && _current->path() == sender->path())
                updateCurrentDownload();
            break;
        case AppDownload::RESUMED:
            // only resume the download in the update method
            updateCurrentDownload();
            break;
        case AppDownload::CANCELED:
            // cancel and remove the download
            sender->cancelDownload();
            if (_current != NULL && _current->path() == sender->path())
                updateCurrentDownload();
            break;
        case AppDownload::FINISHED:
            // remove the registered object in dbus, remove the download and the adapter from the list
            if (_current != NULL && _current->path() == sender->path())
                updateCurrentDownload();
            break;
        default:
            // do nothing
            break;
    }
}

void DownloaderPrivate::loadPreviewsDownloads(QString path)
{
    // TODO
    // list the dirs of the different downloads that we can find, loop and create each of them
    Q_UNUSED(path);
}

QDBusObjectPath DownloaderPrivate::createDownload(const QString &appId, const QString &appName, const QString &url)
{
    return createDownloadWithHash(appId, appName, url, "", QCryptographicHash::Md5);
}

QDBusObjectPath DownloaderPrivate::createDownloadWithHash(const QString &appId, const QString &appName, const QString &url,
    const QString &hash, QCryptographicHash::Algorithm algo)
{
    Q_Q(Downloader);

    // only create a download if the application is not already being downloaded
    QString path = DownloaderPrivate::BASE_ACCOUNT_URL.arg(appId);
    QDBusObjectPath objectPath;

    if (_adaptors.contains(path))
    {
        qDebug() << "Download for app id" << appId << "is already present";
        objectPath = QDBusObjectPath(path);
    }
    else
    {
        AppDownload* appDownload;
        if (hash.isEmpty())
        {
            qDebug() << "Creating AppDownload object for" << appId;
            appDownload = getApplication(appId, appName, url);
        }
        else
        {
            qDebug() << "Creating AppDownload object for " << url << "hash" << hash << "algo" << algo;
            appDownload = getApplication(appId, appName, url, hash, algo);
        }

        ApplicationDownloadAdaptor* adaptor = new ApplicationDownloadAdaptor(appDownload);

        // we need to store the ref of both objects, else the mem management will delete them
        _downloads.append(appDownload);
        _adaptors[appDownload->path()] = adaptor;
        bool ret = _conn->registerObject(appDownload->path(), appDownload);
        qDebug() << "New DBus object registered to " << appDownload->path() << ret;
        objectPath = QDBusObjectPath(appDownload->path());
    }

    // emit that the download was created. Usefull in case other processes are interested in them
    emit q->downloadCreated(objectPath);
    return objectPath;
}

QList<QDBusObjectPath> DownloaderPrivate::getAllDownloads()
{
    qDebug() << "Getting all object paths.";
    QList<QDBusObjectPath> paths;
    foreach(AppDownload* appDownload, _downloads)
        paths << QDBusObjectPath(appDownload->path());
    return paths;
}

/**
 * PUBLIC IMPLEMENTATION
 */

Downloader::Downloader(DBusConnection* connection, QObject *parent) :
    QObject(parent),
    d_ptr(new DownloaderPrivate(connection, this))
{
}

void Downloader::loadPreviewsDownloads(const QString &path)
{
    Q_D(Downloader);
    d->loadPreviewsDownloads(path);
}

QDBusObjectPath Downloader::createDownload(const QString &appId, const QString &appName, const QString &url)
{
    Q_D(Downloader);
    return d->createDownload(appId, appName, url);
}

QDBusObjectPath Downloader::createDownloadWithHash(const QString &appId, const QString &appName, const QString &url,
    const QString &algorithm, const QString &hash)
{
    Q_D(Downloader);
    // lowercase the algorithm just in case
    QString algoLower = algorithm.toLower();
    if (algoLower == "md4")
    {
        return d->createDownloadWithHash(appId, appName, url, hash, QCryptographicHash::Md4);
    }
    else if (algoLower == "md5")
    {
        return d->createDownloadWithHash(appId, appName, url, hash, QCryptographicHash::Md5);
    }
    else if (algoLower == "sha1")
    {
        return d->createDownloadWithHash(appId, appName, url, hash, QCryptographicHash::Sha1);
    }
    else if (algoLower == "sha224")
    {
        return d->createDownloadWithHash(appId, appName, url, hash, QCryptographicHash::Sha224);
    }
    else if (algoLower == "sha256")
    {
        return d->createDownloadWithHash(appId, appName, url, hash, QCryptographicHash::Sha256);
    }
    else if (algoLower == "sha384")
    {
        return d->createDownloadWithHash(appId, appName, url, hash, QCryptographicHash::Sha384);
    }
    else if (algoLower == "sha512")
    {
        return d->createDownloadWithHash(appId, appName, url, hash, QCryptographicHash::Sha512);
    }
    else
    {
        // TODO: what do do here?
        return d->createDownload(appId, appName, url);
    }
}

QList<QDBusObjectPath> Downloader::getAllDownloads()
{
    Q_D(Downloader);
    return d->getAllDownloads();
}

#include "moc_downloader.cpp"
