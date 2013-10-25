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
#include "downloads/queue.h"
#include "downloads/manager.h"
#include "system/apparmor.h"
#include "system/request_factory.h"
#include "system/system_network_info.h"

namespace Ubuntu {

namespace DownloadManager {

/**
 * PRIVATE IMPLEMENATION
 */

typedef std::function<Download*(QString)> DownloadCreationFunc;

class ManagerPrivate {
    Q_DECLARE_PUBLIC(Manager)

 public:
    ManagerPrivate(QSharedPointer<Application> app,
                   QSharedPointer<DBusConnection> connection,
                   bool stoppable,
                   Manager* parent)
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
        _downloadFactory = QSharedPointer<Factory>(
            new Factory(_apparmor, _networkInfo, nam,
                _processFactory));
        _downloadsQueue = QSharedPointer<Queue>(
            new Queue(_networkInfo));
        init();
    }

    ManagerPrivate(QSharedPointer<Application> app,
                   QSharedPointer<DBusConnection> connection,
                   SystemNetworkInfo* networkInfo,
                   Factory* downloadFactory,
                   Queue* queue,
                   bool stoppable,
                   Manager* parent)
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
        Q_Q(Manager);

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
        Q_Q(Manager);
        emit q->sizeChanged(_downloadsQueue->size());
    }

    QDBusObjectPath registerDownload(Download* download) {
        Q_Q(Manager);
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
        Q_Q(Manager);
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
        Q_Q(Manager);
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
    QSharedPointer<Factory> _downloadFactory;
    QSharedPointer<Queue> _downloadsQueue;
    QSharedPointer<DBusConnection> _conn;
    bool _stoppable;
    Manager* q_ptr;
};

/**
 * PUBLIC IMPLEMENTATION
 */

Manager::Manager(QSharedPointer<Application> app,
                 QSharedPointer<DBusConnection> connection,
                 bool stoppable,
                 QObject* parent)
    : QObject(parent),
      d_ptr(new ManagerPrivate(app, connection, stoppable, this)) {
}

Manager::Manager(QSharedPointer<Application> app,
                 QSharedPointer<DBusConnection> connection,
                 SystemNetworkInfo* networkInfo,
                 Factory* downloadFactory,
                 Queue* queue,
                 bool stoppable,
                 QObject* parent)
    : QObject(parent),
      QDBusContext(),
      d_ptr(new ManagerPrivate(app,
                connection,
                networkInfo,
                downloadFactory,
                queue,
                stoppable,
                this)) {
}

void
Manager::loadPreviewsDownloads(const QString &path) {
    Q_D(Manager);
    d->loadPreviewsDownloads(path);
}

QList<QSslCertificate>
Manager::acceptedCertificates() {
    Q_D(Manager);
    return d->acceptedCertificates();
}


void
Manager::setAcceptedCertificates(const QList<QSslCertificate>& certs) {
    Q_D(Manager);
    return d->setAcceptedCertificates(certs);
}

qulonglong
Manager::defaultThrottle() {
    Q_D(Manager);
    return d->defaultThrottle();
}

void
Manager::setDefaultThrottle(qulonglong speed) {
    Q_D(Manager);
    d->setDefaultThrottle(speed);
}

QDBusObjectPath
Manager::createDownload(DownloadStruct download) {
    Q_D(Manager);
    return d->createDownload(download.getUrl(), download.getHash(),
        download.getAlgorithm(), download.getMetadata(), download.getHeaders());
}

QDBusObjectPath
Manager::createDownloadGroup(StructList downloads,
                             const QString& algorithm,
                             bool allowed3G,
                             const QVariantMap& metadata,
                             StringMap headers) {
    Q_D(Manager);
    return d->createDownloadGroup(downloads, algorithm, allowed3G, metadata,
        headers);
}

QList<QDBusObjectPath>
Manager::getAllDownloads() {
    Q_D(Manager);
    return d->getAllDownloads();
}

QList<QDBusObjectPath>
Manager::getAllDownloadsWithMetadata(const QString &name,
                                     const QString &value) {
    Q_D(Manager);
    return d->getAllDownloadsWithMetadata(name, value);
}

void
Manager::exit() {
    Q_D(Manager);
    return d->exit();
}

}  // DownloadManager

}  // Ubuntu

#include "moc_manager.cpp"
