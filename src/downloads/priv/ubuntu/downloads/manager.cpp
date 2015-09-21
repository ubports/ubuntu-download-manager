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
#include <glog/logging.h>
#include <ubuntu/download_manager/system/logger.h>
#include <ubuntu/transfers/system/apparmor.h>
#include <ubuntu/transfers/system/logger.h>
#include <ubuntu/transfers/system/request_factory.h>
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
    _downloadFactory = new Factory(new System::AppArmor(connection), this);
    _db = DownloadsDb::instance();
    _queue = new Queue(this);
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
      _queue(queue) {
    _db = DownloadsDb::instance();
    _conn = connection;
    init();
}

DownloadManager::~DownloadManager() {
    delete _queue;
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
    qDBusRegisterMetaType<HashErrorStruct>();
    qDBusRegisterMetaType<NetworkErrorStruct>();
    qDBusRegisterMetaType<ProcessErrorStruct>();

    CHECK(connect(_queue, &Queue::transferRemoved,
        this, &DownloadManager::onDownloadsChanged))
            << "Could not connect to signal";
    CHECK(connect(_queue, &Queue::transferAdded,
        this, &DownloadManager::onDownloadsChanged))
            << "Could not connect to signal";
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
    emit sizeChanged(_queue->size());
}

QString
DownloadManager::getCaller() {
    QString caller = "";

    bool wasCalledFromDBus = calledFromDBus();
    if (wasCalledFromDBus) {
        caller = connection().interface()->serviceOwner(
            message().service());
        LOG(INFO) << "Owner is: " << caller;
    }
    return caller;
}

QString
DownloadManager::getDownloadOwner(const QVariantMap& metadata) {
    QScopedPointer<System::AppArmor> appArmor(new System::AppArmor(_conn));
    auto owner = getCaller();
    auto appId = appArmor->appId(owner);
    if(appArmor->isConfined(appId)) {
        return appId;
    } else {
        if (metadata.contains(Metadata::APP_ID)){
            return metadata[Metadata::APP_ID].toString();
        } else {
            return "";
        }
    }
    return "";
}

QDBusObjectPath
DownloadManager::registerDownload(Download* download) {
    download->setDownloadOwner(getDownloadOwner(download->metadata()));

    download->setThrottle(_throttle);
    download->allowGSMDownload(_allowMobileData);
    if (!_db->store(download)) {
        LOG(WARNING) << download->transferId()
            << "could not be stored in the db";
    }
    _db->connectToDownload(download);
    _queue->add(download);
    auto path = download->path();
    qDebug() << "ADDED TO Q";
    _conn->registerObject(path, download);
    qDebug() << "REGISTER OBJ";
    QDBusObjectPath objectPath = QDBusObjectPath(path);

    // emit that the download was created. Usefull in case other
    // processes are interested in them
    emit downloadCreated(objectPath);
    return objectPath;
}

QDBusObjectPath
DownloadManager::createDownload(DownloadCreationFunc createDownloadFunc) {
    auto owner = getCaller();
    auto download = createDownloadFunc(owner);

    if (calledFromDBus() && !download->isValid()) {
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
                           int port) {
    LOG(INFO) << "Create MMS download == {url:" << url << " hostname: "
        << hostname << " port:" << port << "}";
    DownloadCreationFunc createDownloadFunc =
        [this, url, hostname, port](QString owner) {
        auto download = _downloadFactory->createMmsDownload(owner, url,
            hostname, port);
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
    QHash<QString, Transfer*> downloads = _queue->transfers();
    foreach(const QString& path, downloads.keys()) {
        downloads[path]->setThrottle(speed);
    }
}

void
DownloadManager::allowGSMDownload(bool allowed) {
    _allowMobileData = allowed;
    QHash<QString, Transfer*> downloads = _queue->transfers();
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
    // filter per app id if owner is not "" and the app is confined else
    // return all downloads
    QScopedPointer<System::AppArmor> appArmor(new System::AppArmor(_conn));
    auto owner = getCaller();
    auto appId = appArmor->appId(owner);
    QList<QDBusObjectPath> paths;
    if (appArmor->isConfined(appId)) {
        LOG(INFO) << "Returning downloads for api with id" << appId;
        auto transfers = _queue->transfers();
        foreach(const QString& path, transfers.keys()) {
            auto t = transfers[path];
            if (t->transferAppId() == appId)
                paths << QDBusObjectPath(path);
        }
    } else {
        LOG(INFO) << "Returning all downloads for unconfined app";
        foreach(const QString& path, _queue->paths())
            paths << QDBusObjectPath(path);
    }
    return paths;
}

QList<QDBusObjectPath>
DownloadManager::getAllDownloadsWithMetadata(const QString &name,
                                             const QString &value) {
    // filter per app id if owner is not "" and the app is confined else
    // return all downloads
    QScopedPointer<System::AppArmor> appArmor(new System::AppArmor(_conn));
    auto owner = getCaller();
    auto appId = appArmor->appId(owner);
    auto isConfined = appArmor->isConfined(appId);

    QList<QDBusObjectPath> paths;
    QHash<QString, Transfer*> downloads = _queue->transfers();
    foreach(const QString& path, downloads.keys()) {
        auto down = qobject_cast<Download*>(downloads[path]);
        if (down != nullptr) {
            QVariantMap metadata = down->metadata();
            if (metadata.contains(name)) {
                QVariant data = metadata[name];
                if (data.canConvert(QMetaType::QString)
                        && data.toString() == value) {
                    if (!isConfined || appId == down->transferAppId()) {
                        paths << QDBusObjectPath(path);
                    }
                }
            }
        }
    }
    return paths;
}

DownloadStateStruct
DownloadManager::getDownloadState(const QString &downloadId) {
    return _db->getDownloadState(downloadId);
}

}  // Daemon

}  // DownloadManager

}  // Ubuntu
