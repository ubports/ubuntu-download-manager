/*
 * Copyright 2014 Canonical Ltd.
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
#include <ubuntu/transfers/system/apparmor.h>
#include <ubuntu/transfers/system/logger.h>
#include <ubuntu/transfers/system/request_factory.h>
#include <ubuntu/upload_manager/metatypes.h>
#include "manager.h"

namespace Ubuntu {

using namespace Transfers;

namespace UploadManager {

namespace Daemon {

const QString UploadManager::SERVICE_PATH =
    "com.canonical.applications.Uploader";

UploadManager::UploadManager(Application* app,
                             DBusConnection* connection,
                             bool stoppable,
                             QObject *parent)
    : BaseManager(app, stoppable, parent),
      _throttle(0) {
    _conn = connection;
    RequestFactory::setStoppable(_stoppable);
    _factory = new Factory(this);
    _queue = new Queue(this);
    init();
}

UploadManager::UploadManager(Application* app,
                             DBusConnection* connection,
                             Factory* uploadFactory,
                             Queue* queue,
                             bool stoppable,
                             QObject *parent)
    : BaseManager(app, stoppable, parent),
      _throttle(0),
      _factory(uploadFactory),
      _queue(queue) {
    _conn = connection;
    init();
}

void
UploadManager::init() {
    // register the required types
    qDBusRegisterMetaType<StringMap>();
    qDBusRegisterMetaType<UploadStruct>();
    qDBusRegisterMetaType<AuthErrorStruct>();
    qDBusRegisterMetaType<HttpErrorStruct>();
    qDBusRegisterMetaType<NetworkErrorStruct>();
    qDBusRegisterMetaType<ProcessErrorStruct>();

    connect(_queue, &Queue::transferRemoved,
        this, &UploadManager::onUploadsChanged);
    connect(_queue, &Queue::transferAdded,
        this, &UploadManager::onUploadsChanged);
}

UploadManager::~UploadManager() {
    delete _queue;
    delete _factory;
}

QList<QSslCertificate>
UploadManager::acceptedCertificates() {
    return _factory->acceptedCertificates();
}

void
UploadManager::setAcceptedCertificates(const QList<QSslCertificate>& certs) {
    LOG(INFO) << __PRETTY_FUNCTION__;
    _factory->setAcceptedCertificates(certs);
}

void
UploadManager::allowMobileUpload(bool allowed) {
    _allowMobileData = allowed;
    QHash<QString, Transfer*> uploads = _queue->transfers();
    foreach(const QString& path, uploads.keys()) {
        uploads[path]->allowGSMData(allowed);
    }
}

QDBusObjectPath
UploadManager::createUpload(UploadCreationFunc createUploadFunc) {
    QString owner = "";

    auto wasCalledFromDBus = calledFromDBus();
    if (wasCalledFromDBus) {
        owner = connection().interface()->serviceOwner(
            message().service());
        LOG(INFO) << "Owner is: " << owner;
    }

    auto upload = createUploadFunc(owner);

    if (wasCalledFromDBus && !upload->isValid()) {
        sendErrorReply(QDBusError::InvalidArgs, upload->lastError());
        // the result will be ignored thanks to the sendErrorReply
        return QDBusObjectPath();
    }
    return registerUpload(upload);
}

QDBusObjectPath
UploadManager::createMmsUpload(const QString& url,
                               const QString& file,
                               const QString& hostname,
                               int port) {
    LOG(INFO) << "Create MMS upload == {url:" << url << " filePath: "
        << file << " hostname:" << hostname << " port:" << port << "}";
    UploadCreationFunc createUploadFunc =
        [this, url, file, hostname, port](QString owner) {
        auto upload = _factory->createMmsUpload(owner, url, file, hostname,
            port);
        return upload;
    };
    return createUpload(createUploadFunc);
}

QDBusObjectPath
UploadManager::createUpload(const QString& url,
                            const QString& filePath,
                            const QVariantMap& metadata,
                            StringMap headers) {

    LOG(INFO) << "Create upload == {url:" << url << " filePath: "
        << filePath << " metadata: " << metadata << " headers: "
        << headers << "}";
    UploadCreationFunc createUploadFunc =
        [this, url, filePath, metadata, headers](QString owner) {
        return _factory->createUpload(owner, url, filePath,
            metadata, headers);
    };
    return createUpload(createUploadFunc);
}

QDBusObjectPath
UploadManager::createUpload(UploadStruct uploadStruct) {
    return createUpload(uploadStruct.getUrl(),
        uploadStruct.getFilePath(), uploadStruct.getMetadata(),
        uploadStruct.getHeaders());
}

qulonglong
UploadManager::defaultThrottle() {
    return _throttle;
}

QList<QDBusObjectPath>
UploadManager::getAllUploads() {
    // TODO: Most be implemented
    QList<QDBusObjectPath> uploads;
    return uploads;
}

QList<QDBusObjectPath>
UploadManager::getAllUploadsWithMetadata(const QString& name,
                                         const QString& value) {
    // TODO: Most be implemented
    Q_UNUSED(name);
    Q_UNUSED(value);
    QList<QDBusObjectPath> uploads;
    return uploads;
}

bool
UploadManager::isMobileUploadAllowed() {
    return _allowMobileData;
}

void
UploadManager::setDefaultThrottle(qulonglong speed) {
    _throttle = speed;
    QHash<QString, Transfer*> uploads = _queue->transfers();
    foreach(const QString& path, uploads.keys()) {
        uploads[path]->setThrottle(speed);
    }
}

void
UploadManager::onUploadsChanged(QString path) {
    LOG(INFO) << __PRETTY_FUNCTION__ << path;
    emit sizeChanged(_queue->size());
}

QDBusObjectPath
UploadManager::registerUpload(FileUpload* upload) {
    LOG(INFO) << "Registering upload to path " << upload->path();
    upload->setThrottle(_throttle);
    upload->allowMobileUpload(_allowMobileData);
    _queue->add(upload);
    _conn->registerObject(upload->path(), upload);
    QDBusObjectPath objectPath = QDBusObjectPath(upload->path());

    // emit that the upload was created. Useful in case other
    // processes are interested in them
    emit uploadCreated(objectPath);
    return objectPath;
}

}  // Daemon

}  // UploadManager

}  // Ubuntu
