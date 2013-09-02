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
#include "./download_queue.h"
#include "./hash_algorithm.h"
#include "./system_network_info.h"
#include "./download_manager.h"

/**
 * PRIVATE IMPLEMENATION
 */

class DownloadManagerPrivate {
    Q_DECLARE_PUBLIC(DownloadManager)

 public:
    DownloadManagerPrivate(QSharedPointer<DBusConnection> connection,
                           DownloadManager* parent)
        : _throttle(0),
          q_ptr(parent) {
        _conn = connection;
        _networkInfo = new SystemNetworkInfo();
        _downloadFactory = new DownloadFactory();
        _downloadsQueue = new DownloadQueue(_networkInfo);
        init();
    }

    DownloadManagerPrivate(QSharedPointer<DBusConnection> connection,
                           SystemNetworkInfo* networkInfo,
                           DownloadFactory* downloadFactory,
                           DownloadQueue* queue,
                           DownloadManager* parent = 0)
        : _throttle(0),
          _networkInfo(networkInfo),
          _downloadFactory(downloadFactory),
          _downloadsQueue(queue),
          q_ptr(parent) {
        _conn = connection;
        init();
    }

 private:
    void init() {
        Q_Q(DownloadManager);

        // register the required types
        qDBusRegisterMetaType<StringMap>();
        qDBusRegisterMetaType<GroupDownloadStruct>();
        qDBusRegisterMetaType<StructList>();

        q->connect(_downloadsQueue, SIGNAL(downloadRemoved(QString)),
            q, SLOT(onDownloadsChanged(QString)));
        q->connect(_downloadsQueue, SIGNAL(downloadAdded(QString)),
            q, SLOT(onDownloadsChanged(QString)));
    }

    void addDownload(Download* download) {
        Q_UNUSED(download);
        // TODO(mandel): Implement this.
    }

    void loadPreviewsDownloads(QString path) {
        // TODO(mandel): list the dirs of the different downloads that we
        // can find, loop and create each of them
        Q_UNUSED(path);
    }

    void onDownloadsChanged(QString path) {
        qDebug() << __FUNCTION__ << path;
        Q_Q(DownloadManager);
        emit q->sizeChanged(_downloadsQueue->size());
    }

    QDBusObjectPath registerDownload(Download* download) {
        Q_Q(DownloadManager);
        download->setThrottle(_throttle);
        _downloadsQueue->add(download);
        _conn->registerObject(download->path(), download);
        QDBusObjectPath objectPath = QDBusObjectPath(download->path());

        // emit that the download was created. Usefull in case other
        // processes are interested in them
        emit q->downloadCreated(objectPath);
        return objectPath;
    }

    QDBusObjectPath createDownload(const QString& url,
                                   const QVariantMap& metadata,
                                   StringMap headers) {
        qDebug() << __FUNCTION__ << url << metadata << headers;
        return createDownloadWithHash(url, "", QCryptographicHash::Md5,
            metadata, headers);
    }

    QDBusObjectPath createDownloadWithHash(const QString& url,
                                           const QString& hash,
                                           QCryptographicHash::Algorithm algo,
                                           const QVariantMap& metadata,
                                           StringMap headers) {
        Download* download = NULL;
        if (hash.isEmpty())
            download = _downloadFactory->createDownload(url, metadata,
                headers);
        else
            download = _downloadFactory->createDownload(url, hash, algo,
                metadata, headers);
        return registerDownload(download);
    }

    QDBusObjectPath createDownloadGroup(StructList downloads,
                                        QCryptographicHash::Algorithm algo,
                                        bool allowed3G,
                                        const QVariantMap& metadata,
                                        StringMap headers) {
        QDBusObjectPath objectPath;
        Download* download = _downloadFactory->createDownload(downloads, algo,
            allowed3G, metadata, headers);
        return registerDownload(download);
    }

    qulonglong defaultThrottle() {
        return _throttle;
    }

    void setDefaultThrottle(qulonglong speed) {
        _throttle = speed;
        QHash<QString, Download*> downloads = _downloadsQueue->downloads();
        foreach(const QString& path, downloads.keys()) {
            downloads[path]->setThrottle(speed);
        }
    }

    QList<QDBusObjectPath> getAllDownloads() {
        QList<QDBusObjectPath> paths;
        foreach(const QString& path, _downloadsQueue->paths())
            paths << QDBusObjectPath(path);
        return paths;
    }

    QList<QDBusObjectPath> getAllDownloadsWithMetadata(const QString &name,
                                                       const QString &value) {
        QList<QDBusObjectPath> paths;
        QHash<QString, Download*> downloads = _downloadsQueue->downloads();
        foreach(const QString& path, downloads.keys()) {
            QVariantMap metadata = downloads[path]->metadata();
            if (metadata.contains(name)) {
                QVariant data = metadata[name];
                if (data.canConvert(QMetaType::QString)
                        && data.toString() == value)
                    paths << QDBusObjectPath(path);
            }
        }
        return paths;
    }

 private:
    qulonglong _throttle;
    SystemNetworkInfo* _networkInfo;
    DownloadFactory* _downloadFactory;
    DownloadQueue* _downloadsQueue;
    QSharedPointer<DBusConnection> _conn;
    DownloadManager* q_ptr;
};

/**
 * PUBLIC IMPLEMENTATION
 */

DownloadManager::DownloadManager(QSharedPointer<DBusConnection> connection,
                                 QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadManagerPrivate(connection, this)) {
}

DownloadManager::DownloadManager(QSharedPointer<DBusConnection> connection,
                                 SystemNetworkInfo* networkInfo,
                                 DownloadFactory* downloadFactory,
                                 DownloadQueue* queue,
                                 QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadManagerPrivate(connection,
                                       networkInfo,
                                       downloadFactory,
                                       queue,
                                       this)) {
}

void
DownloadManager::loadPreviewsDownloads(const QString &path) {
    Q_D(DownloadManager);
    d->loadPreviewsDownloads(path);
}

QDBusObjectPath
DownloadManager::createDownload(const QString &url,
                                const QVariantMap &metadata,
                                StringMap headers) {
    Q_D(DownloadManager);
    return d->createDownload(url, metadata, headers);
}

qulonglong
DownloadManager::defaultThrottle() {
    Q_D(DownloadManager);
    return d->defaultThrottle();
}

void
DownloadManager::setDefaultThrottle(qulonglong speed) {
    Q_D(DownloadManager);
    d->setDefaultThrottle(speed);
}

QDBusObjectPath
DownloadManager::createDownloadWithHash(const QString &url,
                                        const QString &algorithm,
                                        const QString &hash,
                                        const QVariantMap &metadata,
                                        StringMap headers) {
    Q_D(DownloadManager);
    return d->createDownloadWithHash(url, hash,
        HashAlgorithm::getHashAlgo(algorithm), metadata, headers);
}

QDBusObjectPath
DownloadManager::createDownloadGroup(StructList downloads,
                                     const QString& algorithm,
                                     bool allowed3G,
                                     const QVariantMap& metadata,
                                     StringMap headers) {
    Q_D(DownloadManager);
    return d->createDownloadGroup(downloads,
        HashAlgorithm::getHashAlgo(algorithm), allowed3G, metadata, headers);
}

QList<QDBusObjectPath>
DownloadManager::getAllDownloads() {
    Q_D(DownloadManager);
    return d->getAllDownloads();
}

QList<QDBusObjectPath>
DownloadManager::getAllDownloadsWithMetadata(const QString &name,
                                             const QString &value) {
    Q_D(DownloadManager);
    return d->getAllDownloadsWithMetadata(name, value);
}

#include "moc_download_manager.cpp"
