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

#include <QDir>
#include <QFileInfo>
#include <ubuntu/transfers/system/logger.h>
#include <ubuntu/transfers/system/filename_mutex.h>

#include "file_upload.h"

#define UP_LOG(LEVEL) LOG(LEVEL) << "Upload ID{" << objectName() << "}"

namespace {
    const QString CONTENT_TYPE_HEADER = "multipart/form-data;";
    const QString NETWORK_ERROR = "NETWORK ERROR";
    const QString SSL_ERROR = "SSL ERROR";
    const QString AUTH_ERROR = "AUTHENTICATION ERROR";
    const QString PROXY_AUTH_ERROR = "PROXY_AUTHENTICATION ERROR";
    const QString UNEXPECTED_ERROR = "UNEXPECTED_ERROR";
    const QString RESPONSE_EXTENSION = ".response";
}

namespace Ubuntu {

using namespace Transfers::Errors;

namespace UploadManager {

namespace Daemon {

FileUpload::FileUpload(const QString& id,
               const QString& appId,
               const QString& path,
               bool isConfined,
               const QString& rootPath,
               const QUrl& url,
               const QString& filePath,
               const QVariantMap& metadata,
               const QMap<QString, QString>& headers,
               QObject* parent)
    : Transfer(id, appId, path, isConfined, rootPath, parent),
      _url(url),
      _filePath(filePath),
      _metadata(metadata),
      _headers(headers) {

    // we must make sure that the path is absolute
    QFileInfo info(filePath);
    if (!info.isAbsolute()) {
        UP_LOG(INFO) << "Path is not absolute: " << filePath;
        setIsValid(false);
        setLastError(QString("Path is not absolute: '%1'").arg(filePath));
    }

    if (isValid() && !info.exists()) {
        UP_LOG(INFO) << "Path does not exist: " << filePath;
        setIsValid(false);
        setLastError(QString("Path does not exist: '%1'").arg(filePath));
    }

    if (isValid()) {
        _currentData = FileManager::instance()->createFile(filePath);
    }

    _requestFactory = RequestFactory::instance();
}

FileUpload::~FileUpload() {
    if (_currentData != nullptr) {
        _currentData->close();
    }
    delete _currentData;
    delete _reply;
}

QObject*
FileUpload::adaptor() const {
    return _adaptor;
}

void
FileUpload::setAdaptor(QObject* adaptor) {
    _adaptor = adaptor;
}

bool
FileUpload::pausable() {
    return false;
}

void
FileUpload::cancelTransfer() {
    TRACE << _url;

    if (_reply != nullptr) {
        // disconnect so that we do not get useless signals
        // and remove the reply
        disconnectFromReplySignals();
        _reply->abort();
        _reply->deleteLater();
        _reply = nullptr;
    }

    // remove current data and metadata
    emit canceled(true);
}

void
FileUpload::pauseTransfer() {
    UP_LOG(WARNING) << "Uploads cannot be paused!";
}

void
FileUpload::resumeTransfer() {
    UP_LOG(WARNING) << "Uploads cannot be resumed!";
}

void
FileUpload::startTransfer() {
    TRACE << _url;

    if (_reply != nullptr) {
        // the download was already started, lets say that we did it
        UP_LOG(INFO) << "Cannot start download because reply != NULL";
        UP_LOG(INFO) << "EMIT started(false)";
        emit started(true);
        return;
    }

    bool canRead = _currentData->open(QIODevice::ReadOnly);

    if (!canRead) {
        emit started(false);
        return;
    }

    _reply = _requestFactory->post(buildRequest(), _currentData);
    _reply->setReadBufferSize(throttle());

    connectToReplySignals();
    UP_LOG(INFO) << "EMIT started(true)";
    emit started(true);
}

void
FileUpload::allowMobileUpload(bool allowed) {
    // rename the transfer method
    Transfer::allowGSMData(allowed);
}

bool
FileUpload::isMobileUploadAllowed() {
    return Transfer::isGSMDataAllowed();
}

QVariantMap
FileUpload::metadata() {
    return _metadata;
}

qulonglong
FileUpload::progress() {
    return _progress;
}

void
FileUpload::setThrottle(qulonglong speed) {
    TRACE << _url;
    Transfer::setThrottle(speed);
    if (_reply != nullptr)
        _reply->setReadBufferSize(speed);
}

QNetworkRequest
FileUpload::setRequestHeaders(QNetworkRequest request) {
    return request;
}

QNetworkRequest
FileUpload::buildRequest() {
    QNetworkRequest request = QNetworkRequest(_url);
    foreach(const QString& header, _headers.keys()) {
        QString data = _headers[header];
        request.setRawHeader(header.toUtf8(), data.toUtf8());
    }
    auto r = setRequestHeaders(request);
    DLOG(INFO) << "Headers are:" << r.rawHeaderList();
    return r;
}

void
FileUpload::connectToReplySignals() {
    if (_reply != nullptr) {
        connect(_reply, &NetworkReply::uploadProgress,
            this, &FileUpload::onUploadProgress);
        connect(_reply, &NetworkReply::error,
            this, &FileUpload::onError);
        connect(_reply, &NetworkReply::finished,
            this, &FileUpload::onFinished);
        connect(_reply, &NetworkReply::sslErrors,
            this, &FileUpload::onSslErrors);
    }
}

void
FileUpload::disconnectFromReplySignals() {
    if (_reply != nullptr) {
        disconnect(_reply, &NetworkReply::uploadProgress,
            this, &FileUpload::onUploadProgress);
        disconnect(_reply, &NetworkReply::error,
            this, &FileUpload::onError);
        disconnect(_reply, &NetworkReply::finished,
            this, &FileUpload::onFinished);
        disconnect(_reply, &NetworkReply::sslErrors,
            this, &FileUpload::onSslErrors);
    }
}

void
FileUpload::emitError(const QString& errorStr) {
    TRACE << errorStr;
    disconnectFromReplySignals();
    _reply->deleteLater();
    _reply = nullptr;
    setState(Transfer::ERROR);
    emit error(errorStr);
}

void
FileUpload::onUploadProgress(qint64 currentProgress, qint64) {
    _progress = currentProgress;
    emit progress(_progress, _currentData->size());
}

void
FileUpload::onError(QNetworkReply::NetworkError code) {
    UP_LOG(ERROR) << _url << " ERROR:" << ":" << code << " " <<
        QString(_reply->readAll());

    QString msg;
    QString errStr;

    // decide if we are talking about an http error or no
    auto statusCode = _reply->attribute(
        QNetworkRequest::HttpStatusCodeAttribute);
    if (statusCode.isValid()) {
        auto status = statusCode.toInt();
        if (status >= 300) {
            auto reasonVar = _reply->attribute(
                QNetworkRequest::HttpReasonPhraseAttribute);
            if (reasonVar.isValid()) {
                msg = reasonVar.toString();
            } else {
                msg = "";
            }
            HttpErrorStruct err(status, msg);
            emit httpError(err);
            errStr = NETWORK_ERROR;
        }
    } else {
        if (code == QNetworkReply::AuthenticationRequiredError) {
            AuthErrorStruct err(AuthErrorStruct::Server, _reply->errorString());
            emit authError(err);
            errStr = AUTH_ERROR;
        } else if (code == QNetworkReply::ProxyAuthenticationRequiredError) {
            AuthErrorStruct err(AuthErrorStruct::Proxy, _reply->errorString());
            emit authError(err);
            errStr = PROXY_AUTH_ERROR;
        } else {
            NetworkErrorStruct err(code, _reply->errorString());
            emit networkError(err);
        }
    }
    emitError(errStr);
}

QString
FileUpload::writeResponseToDisk() {
    QFileInfo fi(_filePath);
    auto desiredPath = rootPath() + QDir::separator() +
        fi.fileName() + RESPONSE_EXTENSION;
    auto responsePath = FileNameMutex::instance()->lockFileName(desiredPath);
    QScopedPointer<File> f(FileManager::instance()->createFile(responsePath));

    bool canWrite = f->open(QIODevice::ReadWrite | QFile::Append);
    if (!canWrite) {
        emit error("Could not write to response in disk.");
    }

    f->write(_reply->readAll());
    f->close();
    FileNameMutex::instance()->unlockFileName(desiredPath);
    return responsePath;
}

void
FileUpload::onFinished() {
    setState(Transfer::FINISH);
    // it is important to write the response of the upload to a file for the
    // client to process it
    auto path = writeResponseToDisk();

    UP_LOG(INFO) << "EMIT finished";
    emit finished(path);
    _reply->deleteLater();
    _reply = nullptr;
}

void
FileUpload::onSslErrors(const QList<QSslError>& errors) {
    TRACE << errors;
    if (!_reply->canIgnoreSslErrors(errors)) {
        emitError(SSL_ERROR);
    }
}

}  // Daemon

}  // UploadManager

}  // Ubuntu
