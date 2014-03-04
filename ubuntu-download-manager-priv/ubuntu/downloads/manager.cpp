/*
 * Copyright 2013-2014 Canonical Ltd.
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
#include <ubuntu/download_manager/system/logger.h>
#include "ubuntu/transfers/system/apparmor.h"
#include "ubuntu/transfers/system/logger.h"
#include "ubuntu/transfers/system/request_factory.h"
#include "manager.h"

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {


const QString DownloadManager::SERVICE_PATH =
    "com.canonical.applications.Downloader";

DownloadManager::DownloadManager(Application* app,
                                 DBusConnection* connection,
                                 bool stoppable,
                                 QObject* parent)
    : BaseManager(app, stoppable, parent),
      _throttle(0) {
    _conn = connection;
    RequestFactory::setStoppable(_stoppable);
    _downloadFactory = new Factory(this);
    _db = DownloadsDb::instance();
    _downloadsQueue = new Queue(this);
    init();
}

DownloadManager::DownloadManager(Application* app,
                                 DBusConnection* connection,
                                 Factory* downloadFactory,
                                 Queue* queue,
                                 bool stoppable,
                                 QObject* parent)
    : BaseManager(app, stoppable, parent),
      _throttle(0),
      _downloadFactory(downloadFactory),
      _downloadsQueue(queue) {
    _db = DownloadsDb::instance();
    _conn = connection;
    init();
}

DownloadManager::~DownloadManager() {
    delete _downloadsQueue;
    delete _downloadFactory;
}

void
DownloadManager::init() {
    // register the required types
    qDBusRegisterMetaType<StringMap>();
    qDBusRegisterMetaType<DownloadStruct>();
    qDBusRegisterMetaType<GroupDownloadStruct>();
    qDBusRegisterMetaType<StructList>();
    qDBusRegisterMetaType<AuthErrorStruct>();
    qDBusRegisterMetaType<HttpErrorStruct>();
    qDBusRegisterMetaType<NetworkErrorStruct>();
    qDBusRegisterMetaType<ProcessErrorStruct>();

    connect(_downloadsQueue, &Queue::transferRemoved,
        this, &DownloadManager::onDownloadsChanged);
    connect(_downloadsQueue, &Queue::transferAdded,
        this, &DownloadManager::onDownloadsChanged);
}

void
DownloadManager::addDownload(Download* download) {
    Q_UNUSED(download);
    // TODO(mandel): Implement this.
}

void
DownloadManager::loadPreviewsDownloads(QString path) {
    // TODO(mandel): list the dirs of the different downloads that we
    // can find, loop and create each of them
    Q_UNUSED(path);
}

QList<QSslCertificate>
DownloadManager::acceptedCertificates() {
    return _downloadFactory->acceptedCertificates();
}

void
DownloadManager::setAcceptedCertificates(const QList<QSslCertificate>& certs) {
    LOG(INFO) << __PRETTY_FUNCTION__;
    _downloadFactory->setAcceptedCertificates(certs);
}

void
DownloadManager::onDownloadsChanged(QString path) {
    LOG(INFO) << __PRETTY_FUNCTION__ << path;
    emit sizeChanged(_downloadsQueue->size());
}

QDBusObjectPath
DownloadManager::registerDownload(Download* download) {
    download->setThrottle(_throttle);
    download->allowGSMDownload(_allowMobileData);
    if (!_db->store(download)) {
        LOG(WARNING) << download->transferId()
            << "could not be stored in the db";
    }
    _db->connectToDownload(download);
    _downloadsQueue->add(download);
    _conn->registerObject(download->path(), download);
    QDBusObjectPath objectPath = QDBusObjectPath(download->path());

    // emit that the download was created. Usefull in case other
    // processes are interested in them
    emit downloadCreated(objectPath);
    return objectPath;
}

QDBusObjectPath
DownloadManager::createDownload(DownloadCreationFunc createDownloadFunc) {
    QString owner = "";

    bool wasCalledFromDBus = calledFromDBus();
    if (wasCalledFromDBus) {
        owner = connection().interface()->serviceOwner(
            message().service());
        LOG(INFO) << "Owner is: " << owner;
    }

    Download* download = createDownloadFunc(owner);

    if (wasCalledFromDBus && !download->isValid()) {
        sendErrorReply(QDBusError::InvalidArgs, download->lastError());
        // the result will be ignored thanks to the sendErrorReply
        return QDBusObjectPath();
    }

    return registerDownload(download);
}

QDBusObjectPath
DownloadManager::createDownload(const QString& url,
                                const QString& hash,
                                const QString& algo,
                                const QVariantMap& metadata,
                                StringMap headers) {
    LOG(INFO) << "Create download == {url:" << url << " hash: " << hash
        << " algo: " << algo << " metadata: " << metadata << " headers: "
        << headers << "}";
    DownloadCreationFunc createDownloadFunc =
        [this, url, hash, algo, metadata, headers](QString owner) {
        Download* download = nullptr;
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

QDBusObjectPath
DownloadManager::createDownload(DownloadStruct download) {
    return createDownload(download.getUrl(), download.getHash(),
        download.getAlgorithm(), download.getMetadata(), download.getHeaders());
}

QDBusObjectPath
DownloadManager::createMmsDownload(const QString& url,
                                   const QString& hostname,
                                   int port,
                                   const QString& username,
                                   const QString& password) {
    LOG(INFO) << "Create MMS download == {url:" << url << " hostname: "
        << hostname << " port:" << port << " username:" << username
        << " pwd: " << password << "}";
    DownloadCreationFunc createDownloadFunc =
        [this, url, hostname, port, username, password](QString owner) {
        auto download = _downloadFactory->createMmsDownload(owner, url,
            hostname, port, username, password);
        return download;
    };
    return createDownload(createDownloadFunc);
}

QDBusObjectPath
DownloadManager::createDownloadGroup(StructList downloads,
                                     const QString& algo,
                                     bool allowed3G,
                                     const QVariantMap& metadata,
                                     StringMap headers) {
    LOG(INFO) << "Create group download == {downloads:" << downloads
        << " algo:" << algo << " allowMobile" << allowed3G << " metadata:"
        << metadata << " headers:"  << headers << "}";
    DownloadCreationFunc createDownloadFunc =
        [this, downloads, algo, allowed3G, metadata,
            headers] (QString owner) {
        Download* download = _downloadFactory->createDownload(owner,
            downloads, algo, allowed3G, metadata, headers);
        return download;
    };

    return createDownload(createDownloadFunc);
}

qulonglong
DownloadManager::defaultThrottle() {
    return _throttle;
}

void
DownloadManager::setDefaultThrottle(qulonglong speed) {
    _throttle = speed;
    QHash<QString, Transfer*> downloads = _downloadsQueue->transfers();
    foreach(const QString& path, downloads.keys()) {
        downloads[path]->setThrottle(speed);
    }
}

void
DownloadManager::allowGSMDownload(bool allowed) {
    _allowMobileData = allowed;
    QHash<QString, Transfer*> downloads = _downloadsQueue->transfers();
    foreach(const QString& path, downloads.keys()) {
        downloads[path]->allowGSMData(allowed);
    }
}

bool
DownloadManager::isGSMDownloadAllowed() {
    return _allowMobileData;
}

QList<QDBusObjectPath>
DownloadManager::getAllDownloads() {
    QList<QDBusObjectPath> paths;
    foreach(const QString& path, _downloadsQueue->paths())
        paths << QDBusObjectPath(path);
    return paths;
}

QList<QDBusObjectPath>
DownloadManager::getAllDownloadsWithMetadata(const QString &name,
                                             const QString &value) {
    QList<QDBusObjectPath> paths;
    QHash<QString, Transfer*> downloads = _downloadsQueue->transfers();
    foreach(const QString& path, downloads.keys()) {
        auto down = qobject_cast<Download*>(downloads[path]);
        if (down != nullptr) {
            QVariantMap metadata = down->metadata();
            if (metadata.contains(name)) {
                QVariant data = metadata[name];
                if (data.canConvert(QMetaType::QString)
                        && data.toString() == value)
                    paths << QDBusObjectPath(path);
            }
        }
    }
    return paths;
}

}  // Daemon

}  // DownloadManager

}  // Ubuntu
