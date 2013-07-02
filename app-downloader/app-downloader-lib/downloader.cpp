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
#include "download_queue.h"
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

    void addDownload(AppDownload* download);
    void loadPreviewsDownloads(QString path);
    void onDownloadRemoved(QString path);

    QDBusObjectPath createDownload(const QString &appId, const QString &appName, const QString &url);
    QDBusObjectPath createDownloadWithHash(const QString &appId, const QString &appName, const QString &url,
        const QString &hash, QCryptographicHash::Algorithm algo);
    QList<QDBusObjectPath> getAllDownloads();

private:
    static QString BASE_ACCOUNT_URL;

    DownloadQueue* _downloads;
    DBusConnection* _conn;
    RequestFactory* _reqFactory;
    AppDownload* _current;
    Downloader* q_ptr;
};

QString DownloaderPrivate::BASE_ACCOUNT_URL = "/com/canonical/applications/download/%1";

DownloaderPrivate::DownloaderPrivate(DBusConnection* connection, Downloader* parent):
    _conn(connection),
    q_ptr(parent)
{
    Q_Q(Downloader);

    _downloads = new DownloadQueue();
    q->connect(_downloads, SIGNAL(downloadRemoved(QString)),
        q, SLOT(onDownloadRemoved(QString)));

    _reqFactory = new RequestFactory();
    _current = NULL;
}

void DownloaderPrivate::addDownload(AppDownload* download)
{
    Q_UNUSED(download);
    // TODO
}

void DownloaderPrivate::loadPreviewsDownloads(QString path)
{
    // TODO
    // list the dirs of the different downloads that we can find, loop and create each of them
    Q_UNUSED(path);
}

void DownloaderPrivate::onDownloadRemoved(QString path)
{
    _conn->unregisterObject(path);
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

    if (_downloads->paths().contains(path))
        objectPath = QDBusObjectPath(path);
    else
    {
        AppDownload* appDownload;
        if (hash.isEmpty())
            appDownload = new AppDownload(appId, appName, path, url, _reqFactory);
        else
            appDownload = new AppDownload(appId, appName, path, url, hash, algo, _reqFactory);

        ApplicationDownloadAdaptor* adaptor = new ApplicationDownloadAdaptor(appDownload);

        // we need to store the ref of both objects, else the mem management will delete them
        _downloads->add(appDownload, adaptor);
        _conn->registerObject(appDownload->path(), appDownload);
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
    foreach(const QString& path, _downloads->paths())
        paths << QDBusObjectPath(path);
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
