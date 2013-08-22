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
#include "./download_adaptor.h"
#include "./download_queue.h"
#include "./download_manager.h"
#include "./process_factory.h"
#include "./request_factory.h"
#include "./system_network_info.h"

/**
 * PRIVATE IMPLEMENATION
 */

class DownloadManagerPrivate {
    Q_DECLARE_PUBLIC(DownloadManager)

 public:
    explicit DownloadManagerPrivate(QSharedPointer<DBusConnection> connection,
                                    DownloadManager* parent);
    explicit DownloadManagerPrivate(QSharedPointer<DBusConnection> connection,
                                    SystemNetworkInfo* networkInfo,
                                    DownloadQueue* queue,
                                    UuidFactory* uuidFactory,
                                    DownloadManager* parent = 0);

 private:
    void init();
    void addDownload(Download* download);
    void loadPreviewsDownloads(QString path);
    void onDownloadsChanged(QString path);

    QDBusObjectPath createDownload(const QString& url,
                                   const QVariantMap& metadata,
                                   StringMap headers);
    QDBusObjectPath createDownloadWithHash(const QString& url,
                                           const QString& hash,
                                           QCryptographicHash::Algorithm algo,
                                           const QVariantMap& metadata,
                                           StringMap headers);
    qulonglong defaultThrottle();
    void setDefaultThrottle(qulonglong speed);
    QList<QDBusObjectPath> getAllDownloads();
    QList<QDBusObjectPath> getAllDownloadsWithMetadata(const QString &name,
                                                       const QString &value);

 private:
    static QString BASE_ACCOUNT_URL;

    qulonglong _throttle;
    SystemNetworkInfo* _networkInfo;
    DownloadQueue* _downloadsQueue;
    QSharedPointer<DBusConnection> _conn;
    RequestFactory* _reqFactory;
    ProcessFactory* _processFactory;
    UuidFactory* _uuidFactory;
    DownloadManager* q_ptr;
};

QString DownloadManagerPrivate::BASE_ACCOUNT_URL =
    "/com/canonical/applications/download/%1";

DownloadManagerPrivate::DownloadManagerPrivate(
                                QSharedPointer<DBusConnection> connection,
                                DownloadManager* parent)
    : _throttle(0),
      q_ptr(parent) {
    _conn = connection;
    _networkInfo = new SystemNetworkInfo();
    _downloadsQueue = new DownloadQueue(_networkInfo);
    _uuidFactory = new UuidFactory();
    init();
}

DownloadManagerPrivate::DownloadManagerPrivate(
                                QSharedPointer<DBusConnection> connection,
                                SystemNetworkInfo* networkInfo,
                                DownloadQueue* queue,
                                UuidFactory* uuidFactory,
                                DownloadManager* parent)
        : _throttle(0),
          _networkInfo(networkInfo),
          _downloadsQueue(queue),
          _uuidFactory(uuidFactory),
          q_ptr(parent) {
    _conn = connection;
    init();
}

void
DownloadManagerPrivate::init() {
    Q_Q(DownloadManager);

    // register the required types
    qDBusRegisterMetaType<StringMap>();

    q->connect(_downloadsQueue, SIGNAL(downloadRemoved(QString)),
        q, SLOT(onDownloadsChanged(QString)));
    q->connect(_downloadsQueue, SIGNAL(downloadAdded(QString)),
        q, SLOT(onDownloadsChanged(QString)));

    _reqFactory = new RequestFactory();
    _processFactory = new ProcessFactory();
}

void
DownloadManagerPrivate::addDownload(Download* download) {
    Q_UNUSED(download);
    // TODO(mandel): Implement this.
}

void
DownloadManagerPrivate::loadPreviewsDownloads(QString path) {
    // TODO(mandel): list the dirs of the different downloads that we
    // can find, loop and create each of them
    Q_UNUSED(path);
}

void
DownloadManagerPrivate::onDownloadsChanged(QString path) {
    qDebug() << __FUNCTION__ << path;
    Q_Q(DownloadManager);
    emit q->sizeChanged(_downloadsQueue->size());
}

QDBusObjectPath
DownloadManagerPrivate::createDownload(const QString& url,
                                       const QVariantMap& metadata,
                                       StringMap headers) {
    qDebug() << __FUNCTION__ << url << metadata << headers;
    return createDownloadWithHash(url, "", QCryptographicHash::Md5,
        metadata, headers);
}

QDBusObjectPath
DownloadManagerPrivate::createDownloadWithHash(const QString& url,
                                               const QString& hash,
                                               QCryptographicHash::Algorithm algo,  // NOLINT(whitespace/line_length)
                                               const QVariantMap& metadata,
                                               StringMap headers) {
    Q_Q(DownloadManager);

    // only create a download if the application is not already being downloaded
    QUuid id = _uuidFactory->createUuid();
    QString uuidString = id.toString().replace(QRegExp("[-{}]"), "");

    QString path = DownloadManagerPrivate::BASE_ACCOUNT_URL.arg(uuidString);
    QDBusObjectPath objectPath;

    if (_downloadsQueue->paths().contains(path)) {
        objectPath = QDBusObjectPath(path);
    } else {
        Download* download;
        if (hash.isEmpty())
            download = new Download(id, path, url, metadata, headers,
                _networkInfo, _reqFactory, _processFactory);
        else
            download = new Download(id, path, url, hash, algo, metadata,
                headers, _networkInfo, _reqFactory, _processFactory);

        download->setThrottle(_throttle);
        DownloadAdaptor* adaptor = new DownloadAdaptor(download);

        // we need to store the ref of both objects, else the mem management
        // will delete them
        _downloadsQueue->add(download, adaptor);
        _conn->registerObject(download->path(), download);
        objectPath = QDBusObjectPath(download->path());
    }

    // emit that the download was created. Usefull in case other
    // processes are interested in them
    emit q->downloadCreated(objectPath);
    return objectPath;
}

qulonglong
DownloadManagerPrivate::defaultThrottle() {
    return _throttle;
}

void
DownloadManagerPrivate::setDefaultThrottle(qulonglong speed) {
    _throttle = speed;
    QHash<QString, Download*> downloads = _downloadsQueue->downloads();
    foreach(const QString& path, downloads.keys()) {
        downloads[path]->setThrottle(speed);
    }
}


QList<QDBusObjectPath>
DownloadManagerPrivate::getAllDownloads() {
    QList<QDBusObjectPath> paths;
    foreach(const QString& path, _downloadsQueue->paths())
        paths << QDBusObjectPath(path);
    return paths;
}

QList<QDBusObjectPath>
DownloadManagerPrivate::getAllDownloadsWithMetadata(const QString& name,
                                                    const QString& value) {
    QList<QDBusObjectPath> paths;
    QHash<QString, Download*> downloads = _downloadsQueue->downloads();
    foreach(const QString& path, downloads.keys()) {
        QVariantMap metadata = downloads[path]->metadata();
        if (metadata.contains(name)) {
            QVariant data = metadata[name];
            if (data.canConvert(QMetaType::QString) && data.toString() == value)
                paths << QDBusObjectPath(path);
        }
    }
    return paths;
}

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
                                 DownloadQueue* queue,
                                 UuidFactory* uuidFactory,
                                 QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadManagerPrivate(connection,
                                       networkInfo,
                                       queue,
                                       uuidFactory,
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
    // lowercase the algorithm just in case
    QString algoLower = algorithm.toLower();
    QCryptographicHash::Algorithm algo = QCryptographicHash::Md5;

    if (algoLower == "md5")
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
