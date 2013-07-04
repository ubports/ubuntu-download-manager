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

#include <QRegExp>
#include "request_factory.h"
#include "download_adaptor.h"
#include "download_queue.h"
#include "download_manager.h"

/**
 * PRIVATE IMPLEMENATION
 */

class DownloadManagerPrivate
{
    Q_DECLARE_PUBLIC(DownloadManager)
public:
    explicit DownloadManagerPrivate(DBusConnection* connection, DownloadManager* parent);

private:

    void addDownload(Download* download);
    void loadPreviewsDownloads(QString path);
    void onDownloadRemoved(QString path);

    QDBusObjectPath createDownload(const QString& url, const QVariantMap& metadata, const QVariantMap& headers);
    QDBusObjectPath createDownloadWithHash(const QString& url, const QString& hash, QCryptographicHash::Algorithm algo,
        const QVariantMap& metadata, const QVariantMap& headers);
    QList<QDBusObjectPath> getAllDownloads();
    QList<QDBusObjectPath> getAllDownloadsWithMetadata(const QString &name, const QString &value);

private:
    static QString BASE_ACCOUNT_URL;

    DownloadQueue* _downloads;
    DBusConnection* _conn;
    RequestFactory* _reqFactory;
    Download* _current;
    DownloadManager* q_ptr;
};

QString DownloadManagerPrivate::BASE_ACCOUNT_URL = "/com/canonical/applications/download/%1";

DownloadManagerPrivate::DownloadManagerPrivate(DBusConnection* connection, DownloadManager* parent):
    _conn(connection),
    q_ptr(parent)
{
    Q_Q(DownloadManager);

    _downloads = new DownloadQueue();
    q->connect(_downloads, SIGNAL(downloadRemoved(QString)),
        q, SLOT(onDownloadRemoved(QString)));

    _reqFactory = new RequestFactory();
    _current = NULL;
}

void DownloadManagerPrivate::addDownload(Download* download)
{
    Q_UNUSED(download);
    // TODO
}

void DownloadManagerPrivate::loadPreviewsDownloads(QString path)
{
    // TODO
    // list the dirs of the different downloads that we can find, loop and create each of them
    Q_UNUSED(path);
}

void DownloadManagerPrivate::onDownloadRemoved(QString path)
{
    _conn->unregisterObject(path);
}

QDBusObjectPath DownloadManagerPrivate::createDownload(const QString& url, const QVariantMap& metadata, const QVariantMap& headers)
{
    return createDownloadWithHash(url, "", QCryptographicHash::Md5, metadata, headers);
}

QDBusObjectPath DownloadManagerPrivate::createDownloadWithHash(const QString& url, const QString& hash,
    QCryptographicHash::Algorithm algo, const QVariantMap& metadata, const QVariantMap& headers)
{
    Q_Q(DownloadManager);

    // only create a download if the application is not already being downloaded
    QUuid id = QUuid::createUuid();
    QString uuidString = id.toString().replace(QRegExp("[-{}]"), "");

    QString path = DownloadManagerPrivate::BASE_ACCOUNT_URL.arg(uuidString);
    QDBusObjectPath objectPath;

    if (_downloads->paths().contains(path))
        objectPath = QDBusObjectPath(path);
    else
    {
        Download* appDownload;
        if (hash.isEmpty())
            appDownload = new Download(id, path, url, metadata, headers, _reqFactory);
        else
            appDownload = new Download(id, path, url, hash, algo, metadata, headers, _reqFactory);

        DownloadAdaptor* adaptor = new DownloadAdaptor(appDownload);

        // we need to store the ref of both objects, else the mem management will delete them
        _downloads->add(appDownload, adaptor);
        _conn->registerObject(appDownload->path(), appDownload);
        objectPath = QDBusObjectPath(appDownload->path());
    }

    // emit that the download was created. Usefull in case other processes are interested in them
    emit q->downloadCreated(objectPath);
    return objectPath;
}

QList<QDBusObjectPath> DownloadManagerPrivate::getAllDownloads()
{
    qDebug() << "Getting all object paths.";
    QList<QDBusObjectPath> paths;
    foreach(const QString& path, _downloads->paths())
        paths << QDBusObjectPath(path);
    return paths;
}

QList<QDBusObjectPath> DownloadManagerPrivate::getAllDownloadsWithMetadata(const QString& name, const QString& value)
{
    // TODO: return something real!
    Q_UNUSED(name);
    Q_UNUSED(value);
    QList<QDBusObjectPath> paths;
    return paths;
}

/**
 * PUBLIC IMPLEMENTATION
 */

DownloadManager::DownloadManager(DBusConnection* connection, QObject *parent) :
    QObject(parent),
    d_ptr(new DownloadManagerPrivate(connection, this))
{
}

void DownloadManager::loadPreviewsDownloads(const QString &path)
{
    Q_D(DownloadManager);
    d->loadPreviewsDownloads(path);
}

QDBusObjectPath DownloadManager::createDownload(const QString &url, const QVariantMap &metadata, const QVariantMap &headers)
{
    Q_D(DownloadManager);
    return d->createDownload(url, metadata, headers);
}

QDBusObjectPath DownloadManager::createDownloadWithHash(const QString &url, const QString &algorithm, const QString &hash,
        const QVariantMap &metadata, const QVariantMap &headers)
{
    Q_D(DownloadManager);
    // lowercase the algorithm just in case
    QString algoLower = algorithm.toLower();
    QCryptographicHash::Algorithm  algo;
    if (algoLower == "md4")
        algo = QCryptographicHash::Md4;
    else if (algoLower == "md5")
        algo = QCryptographicHash::Md5;
    else if (algoLower == "sha1")
        algo = QCryptographicHash::Sha1;
    else if (algoLower == "sha224")
        algo = QCryptographicHash::Sha224;
    else if (algoLower == "sha256")
        algo = QCryptographicHash::Sha256;
    else if (algoLower == "sha384")
        algo = QCryptographicHash::Sha384;
    else if (algoLower == "sha512")
        algo = QCryptographicHash::Sha512;

    return d->createDownloadWithHash(url, hash, algo, metadata, headers);
}

QList<QDBusObjectPath> DownloadManager::getAllDownloads()
{
    Q_D(DownloadManager);
    return d->getAllDownloads();
}

QList<QDBusObjectPath> DownloadManager::getAllDownloadsWithMetadata(const QString &name, const QString &value)
{
    Q_D(DownloadManager);
    return d->getAllDownloadsWithMetadata(name, value);
}

#include "moc_download_manager.cpp"
