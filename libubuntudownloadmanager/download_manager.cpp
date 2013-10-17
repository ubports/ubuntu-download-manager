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

#include <functional>
#include <QRegExp>
#include "apparmor.h"
#include "download_queue.h"
#include "request_factory.h"
#include "system_network_info.h"
#include "download_manager.h"

/**
 * PRIVATE IMPLEMENATION
 */

typedef std::function<Download*(QString)> DownloadCreationFunc;

class DownloadManagerPrivate {
    Q_DECLARE_PUBLIC(DownloadManager)

 public:
    DownloadManagerPrivate(QSharedPointer<Application> app,
                           QSharedPointer<DBusConnection> connection,
                           bool stoppable,
                           DownloadManager* parent)
        : _app(app),
          _throttle(0),
          _stoppable(stoppable),
          q_ptr(parent) {
        _conn = connection;
        _apparmor = QSharedPointer<AppArmor>(new AppArmor(connection));
        _networkInfo = QSharedPointer<SystemNetworkInfo>(
            new SystemNetworkInfo());
        QSharedPointer<RequestFactory> nam = QSharedPointer<RequestFactory>(
            new RequestFactory(_stoppable));
        _processFactory = QSharedPointer<ProcessFactory>(new ProcessFactory());
        _downloadFactory = QSharedPointer<DownloadFactory>(
            new DownloadFactory(_apparmor, _networkInfo, nam,
                _processFactory));
        _downloadsQueue = QSharedPointer<DownloadQueue>(
            new DownloadQueue(_networkInfo));
        init();
    }

    DownloadManagerPrivate(QSharedPointer<Application> app,
                           QSharedPointer<DBusConnection> connection,
                           SystemNetworkInfo* networkInfo,
                           DownloadFactory* downloadFactory,
                           DownloadQueue* queue,
                           bool stoppable,
                           DownloadManager* parent)
        : _app(app),
          _throttle(0),
          _networkInfo(networkInfo),
          _downloadFactory(downloadFactory),
          _downloadsQueue(queue),
          _stoppable(stoppable),
          q_ptr(parent) {
        _conn = connection;
        init();
    }

 private:
    void init() {
        Q_Q(DownloadManager);

        // register the required types
        qDBusRegisterMetaType<StringMap>();
        qDBusRegisterMetaType<DownloadStruct>();
        qDBusRegisterMetaType<GroupDownloadStruct>();
        qDBusRegisterMetaType<StructList>();

        q->connect(_downloadsQueue.data(), SIGNAL(downloadRemoved(QString)),
            q, SLOT(onDownloadsChanged(QString)));
        q->connect(_downloadsQueue.data(), SIGNAL(downloadAdded(QString)),
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

    QList<QSslCertificate> acceptedCertificates() {
        return _downloadFactory->acceptedCertificates();
    }

    void setAcceptedCertificates(const QList<QSslCertificate>& certs) {
        qDebug() << __PRETTY_FUNCTION__ << certs;
        _downloadFactory->setAcceptedCertificates(certs);
    }

    void onDownloadsChanged(QString path) {
        qDebug() << __PRETTY_FUNCTION__ << path;
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

    QDBusObjectPath createDownload(DownloadCreationFunc createDownloadFunc) {
        Q_Q(DownloadManager);
        QString owner = "";

        bool calledFromDBus = q->calledFromDBus();
        if (calledFromDBus) {
            owner = q->connection().interface()->serviceOwner(
                q->message().service());
            qDebug() << "Owner is: " << owner;
        }

        Download* download = createDownloadFunc(owner);

        if (calledFromDBus && !download->isValid()) {
            q->sendErrorReply(QDBusError::InvalidArgs, download->lastError());
            // the result will be ignored thanks to the sendErrorReply
            return QDBusObjectPath();
        }

        return registerDownload(download);
    }

    QDBusObjectPath createDownload(const QString& url,
                                   const QString& hash,
                                   const QString& algo,
                                   const QVariantMap& metadata,
                                   StringMap headers) {
        DownloadCreationFunc createDownloadFunc =
            [this, url, hash, algo, metadata, headers](QString owner) {
            Download* download = NULL;
            if (hash.isEmpty())
                download = _downloadFactory->createDownload(owner, url,
                    metadata, headers);
            else
                download = _downloadFactory->createDownload(owner, url, hash,
                    algo, metadata, headers);
            return download;
        };
        return createDownload(createDownloadFunc);
    }

    QDBusObjectPath createDownloadGroup(StructList downloads,
                                        const QString& algo,
                                        bool allowed3G,
                                        const QVariantMap& metadata,
                                        StringMap headers) {
        DownloadCreationFunc createDownloadFunc =
            [this, downloads, algo, allowed3G, metadata,
                headers] (QString owner) {
            Download* download = _downloadFactory->createDownload(owner,
                downloads, algo, allowed3G, metadata, headers);
            return download;
        };

        return createDownload(createDownloadFunc);
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


    void exit() {
        Q_Q(DownloadManager);
        if (_stoppable) {
            _app->exit(0);
        } else {
            if (q->calledFromDBus()) {
                q->sendErrorReply(QDBusError::NotSupported,
                    "Daemon should have been started with -stoppable");
            }  // dbus call
        }
    }

 private:
    QSharedPointer<Application> _app;
    qulonglong _throttle;
    QSharedPointer<AppArmor> _apparmor;
    QSharedPointer<SystemNetworkInfo> _networkInfo;
    QSharedPointer<ProcessFactory> _processFactory;
    QSharedPointer<DownloadFactory> _downloadFactory;
    QSharedPointer<DownloadQueue> _downloadsQueue;
    QSharedPointer<DBusConnection> _conn;
    bool _stoppable;
    DownloadManager* q_ptr;
};

/**
 * PUBLIC IMPLEMENTATION
 */

DownloadManager::DownloadManager(QSharedPointer<Application> app,
                                 QSharedPointer<DBusConnection> connection,
                                 bool stoppable,
                                 QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadManagerPrivate(app, connection, stoppable, this)) {
}

DownloadManager::DownloadManager(QSharedPointer<Application> app,
                                 QSharedPointer<DBusConnection> connection,
                                 SystemNetworkInfo* networkInfo,
                                 DownloadFactory* downloadFactory,
                                 DownloadQueue* queue,
                                 bool stoppable,
                                 QObject* parent)
    : QObject(parent),
      QDBusContext(),
      d_ptr(new DownloadManagerPrivate(app,
                                       connection,
                                       networkInfo,
                                       downloadFactory,
                                       queue,
                                       stoppable,
                                       this)) {
}

void
DownloadManager::loadPreviewsDownloads(const QString &path) {
    Q_D(DownloadManager);
    d->loadPreviewsDownloads(path);
}

QList<QSslCertificate>
DownloadManager::acceptedCertificates() {
    Q_D(DownloadManager);
    return d->acceptedCertificates();
}


void
DownloadManager::setAcceptedCertificates(const QList<QSslCertificate>& certs) {
    Q_D(DownloadManager);
    return d->setAcceptedCertificates(certs);
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
DownloadManager::createDownload(DownloadStruct download) {
    Q_D(DownloadManager);
    return d->createDownload(download.getUrl(), download.getHash(),
        download.getAlgorithm(), download.getMetadata(), download.getHeaders());
}

QDBusObjectPath
DownloadManager::createDownloadGroup(StructList downloads,
                                     const QString& algorithm,
                                     bool allowed3G,
                                     const QVariantMap& metadata,
                                     StringMap headers) {
    Q_D(DownloadManager);
    return d->createDownloadGroup(downloads, algorithm, allowed3G, metadata,
        headers);
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

void
DownloadManager::exit() {
    Q_D(DownloadManager);
    return d->exit();
}

#include "moc_download_manager.cpp"
