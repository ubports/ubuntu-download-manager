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

#include <QBuffer>
#include <QCryptographicHash>
#include <QDir>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QSslError>
#include <glog/logging.h>
#include <map>
#include <ubuntu/transfers/system/hash_algorithm.h>
#include <ubuntu/transfers/system/cryptographic_hash.h>
#include <ubuntu/transfers/system/logger.h>
#include <ubuntu/transfers/system/network_reply.h>
#include <ubuntu/transfers/system/filename_mutex.h>
#include <ubuntu/transfers/system/uuid_factory.h>
#include <ubuntu/transfers/system/uuid_utils.h>
#include "file_download.h"

#define DOWN_LOG(LEVEL) LOG(LEVEL) << ((parent() != nullptr)?"GroupDownload {" + parent()->objectName() + " } ":"") << "Download ID{" << objectName() << " } "


std::ostream& operator<<(std::ostream &out, QNetworkReply::NetworkError err) {
    static std::map<QNetworkReply::NetworkError, std::string> errorsMap {
        {QNetworkReply::ConnectionRefusedError, "ConnectionRefusedError: the remote "
                "server refused the connection (the server is not accepting "
                "requests)"},
        {QNetworkReply::RemoteHostClosedError, "RemoteHostClosedError: the remote "
                "server closed the connection prematurely, before the entire reply "
                "was received and processed"},
        {QNetworkReply::HostNotFoundError, "HostNotFoundError: the remote host name "
                "was not found (invalid hostname)"},
        {QNetworkReply::TimeoutError, "TimeoutError: the connection to the remote "
                "server timed out"},
        {QNetworkReply::OperationCanceledError, "OperationCanceledError: the "
                "operation was canceled via calls to abort() or close() before "
                "it was finished."},
        {QNetworkReply::SslHandshakeFailedError, "SslHandshakeFailedError: the "
                "SSL/TLS handshake failed and the encrypted channel could not "
                "be established."},
        {QNetworkReply::TemporaryNetworkFailureError, "TemporaryNetworkFailureError: "
                "the connection was broken "
                "due to disconnection from the network, however the system has "
                "initiated roaming to another access point. The request should be "
                "resubmitted and will be processed as soon as the connection is "
                "re-established."},
        {QNetworkReply::NetworkSessionFailedError, "NetworkSessionFailedError: the "
                "connection was broken due to disconnection from the network or "
                "failure to start the network."},
        {QNetworkReply::BackgroundRequestNotAllowedError, 
                "BackgroundRequestNotAllowedError: the background request is "
                "not currently allowed due to platform policy."},
        {QNetworkReply::ProxyConnectionRefusedError, "ProxyConnectionRefusedError: "
                "the connection to the proxy server was refused (the proxy server "
                "is not accepting requests)"},
        {QNetworkReply::ProxyConnectionClosedError, "ProxyConnectionClosedError: the "
                "proxy server closed the connection prematurely, before the entire "
                "reply was received and processed"},
        {QNetworkReply::ProxyNotFoundError, "ProxyNotFoundError: the proxy host name "
                "was not found (invalid proxy hostname)"},
        {QNetworkReply::ProxyTimeoutError, "ProxyTimeoutError: the connection to "
                "the proxy timed out or the proxy did not reply in time to the "
                "request sent"},
        {QNetworkReply::ProxyAuthenticationRequiredError,
                "ProxyAuthenticationRequiredError: the proxy requires "
                "authentication in order to honour the request but did not accept "
                "any credentials offered (if any)"},
        {QNetworkReply::ContentAccessDenied, "ContentAccessDenied: the access to "
                "the remote content was denied (similar to HTTP error 401)"},
        {QNetworkReply::ContentOperationNotPermittedError,
                "ContentOperationNotPermittedError: the operation requested "
                "on the remote content is not permitted"},
        {QNetworkReply::ContentNotFoundError, "ContentNotFoundError: the remote "
                "content was not found at the server (similar to HTTP error 404)"},
        {QNetworkReply::AuthenticationRequiredError, "AuthenticationRequiredError: "
                "the remote server requires authentication to serve the content "
                "but the credentials provided were not accepted (if any)."},
        {QNetworkReply::ContentReSendError, "ContentReSendError: the request needed "
                "to be sent again, but this failed for example because the upload "
                "data could not be read a second time."},
        {QNetworkReply::ContentConflictError, "ContentConflictError: the request "
                "could not be completed due to a conflict with the current state "
                "of the resource."},
        {QNetworkReply::ContentGoneError, "ContentGoneError: the requested resource "
                "is no longer available at the server."},
        {QNetworkReply::InternalServerError, "InternalServerError: the server "
                "encountered an unexpected condition which prevented it from "
                "fulfilling the request."},
        {QNetworkReply::OperationNotImplementedError,
                "OperationNotImplementedError: the server does not support "
                "the functionality required to fulfill the request."},
        {QNetworkReply::ServiceUnavailableError, "ServiceUnavailableError: the "
                "server is unable to handle the request at this time."},
        {QNetworkReply::ProtocolUnknownError, "ProtocolUnknownError: the Network "
                "Access API cannot honor the request because the protocol "
                "is not known"},
        {QNetworkReply::ProtocolInvalidOperationError,
                "ProtocolInvalidOperationError: the requested operation is "
                "invalid for this protocol"},
        {QNetworkReply::UnknownNetworkError,
                "UnknownNetworkError: an unknown network-related error was "
                "detected"},
        {QNetworkReply::UnknownProxyError, "UnknownProxyError: an unknown "
                "proxy-related error was detected"},
        {QNetworkReply::UnknownContentError, "UnknownContentError: an unknown "
                "error related to the remote content was detected"},
        {QNetworkReply::ProtocolFailure, "ProtocolFailure: a breakdown in protocol "
                "was detected (parsing error, invalid or unexpected responses, etc.)"},
        {QNetworkReply::UnknownServerError, "UnknownServerError: an unknown error "
                "related to the server response was detected"}
    };
    out << "Network error " << errorsMap[err];
    return out;
}

namespace {

    const QString DATA_FILE_NAME = "data.download";
    const QString NETWORK_ERROR = "NETWORK ERROR";
    const QString HASH_ERROR = "HASH ERROR";
    const QString COMMAND_ERROR = "COMMAND ERROR";
    const QString SSL_ERROR = "SSL ERROR";
    const QString FILE_SYSTEM_ERROR = "FILE SYSTEM ERROR: %1";
    const QString TEMP_EXTENSION = ".tmp";
    const QString AUTH_ERROR = "AUTHENTICATION ERROR";
    const QString PROXY_AUTH_ERROR = "PROXY_AUTHENTICATION ERROR";
    const QString UNEXPECTED_ERROR = "UNEXPECTED_ERROR";
    const QByteArray CONTENT_DISPOSITION = "Content-Disposition";
}

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

FileDownload::FileDownload(const QString& id,
                   const QString& appId,
                   const QString& path,
                   bool isConfined,
                   const QString& rootPath,
                   const QUrl& url,
                   const QVariantMap& metadata,
                   const QMap<QString, QString>& headers,
                   QObject* parent)
    : Download(id, appId, path, isConfined, rootPath, metadata, headers, parent),
      QDBusContext(),
      _totalSize(0),
      _url(url),
      _hash(""),
      _algo(QCryptographicHash::Md5) {
    init();
}

FileDownload::FileDownload(const QString& id,
                   const QString& appId,
                   const QString& path,
                   bool isConfined,
                   const QString& rootPath,
                   const QUrl& url,
                   const QString& hash,
                   const QString& algo,
                   const QVariantMap& metadata,
                   const QMap<QString, QString> &headers,
                   QObject* parent)
    : Download(id, appId, path, isConfined, rootPath, metadata, headers, parent),
      QDBusContext(),
      _totalSize(0),
      _url(url),
      _hash(hash) {
    init();
    _algo = HashAlgorithm::getHashAlgo(algo);
    // check that the algorithm is correct if the hash is not empty
    if (!_hash.isEmpty() && !HashAlgorithm::isValidAlgo(algo)) {
        setIsValid(false);
        setLastError(QString("Invalid hash algorithm: '%1'").arg(algo));
    }
}

FileDownload::~FileDownload() {
    if (_currentData != nullptr) {
        _currentData->close();
    }
    delete _currentData;
    delete _reply;
}

void
FileDownload::cancelTransfer() {
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
    cleanUpCurrentData();
    // unlock the file name so that other downloads can use it it if is
    // no used in the file system
    unlockFilePath();
    _downloading = false;
    emit canceled(true);
}

void
FileDownload::pauseTransfer() {
    TRACE << _url;
    if (_reply == nullptr) {
        // cannot pause because is not running
        DOWN_LOG(INFO) << "Cannot pause download because reply is NULL";
        DOWN_LOG(INFO) << "EMIT paused(false)";
        emit paused(false);
        return;
    }

    DOWN_LOG(INFO) << "Pausing download" << _url;
    // we need to disconnect the signals to ensure that they are not
    // emitted due to the operation we are going to perform. We read
    // the data in the reply and store it in a file
    disconnectFromReplySignals();

    // do abort before reading
    _reply->abort();
    _currentData->write(_reply->readAll());
    if (!flushFile()) {
        emit paused(false);
    } else {
        _reply->deleteLater();
        _reply = nullptr;
        DOWN_LOG(INFO) << "EMIT paused(true)";
        _downloading = false;
        emit paused(true);
    }
}

void
FileDownload::resumeTransfer() {
    DOWN_LOG(INFO) << __PRETTY_FUNCTION__ << _url;

    if (_reply != nullptr) {
        // cannot resume because it is already running
        DOWN_LOG(INFO) << "Cannot resume download because reply != NULL";
        DOWN_LOG(INFO) << "EMIT resumed(false)";
        emit resumed(false);
        return;
    }

    DOWN_LOG(INFO) << "Resuming download.";
    QNetworkRequest request = buildRequest();

    // overrides the range header, we do not let clients set the range!!!
    qint64 currentDataSize = _currentData->size();
    QByteArray rangeHeaderValue = "bytes=" +
        QByteArray::number(currentDataSize) + "-";
    request.setRawHeader("Range", rangeHeaderValue);

    _reply = _requestFactory->get(request);
    _reply->setReadBufferSize(throttle());

    connectToReplySignals();

    DOWN_LOG(INFO) << "EMIT resumed(true)";
    _downloading = true;
    emit resumed(true);
}

void
FileDownload::startTransfer() {
    TRACE << _url;

    if (_reply != nullptr) {
        // the download was already started, lets say that we did it
        DOWN_LOG(INFO) << "Cannot start download because reply != NULL";
        DOWN_LOG(INFO) << "EMIT started(false)";
        emit started(true);
        return;
    }

    // create file that will be used to maintain the state of the
    // download when resumed.
    _currentData = FileManager::instance()->createFile(_tempFilePath);
    bool canWrite = _currentData->open(QIODevice::ReadWrite | QFile::Append);

    if (!canWrite) {
        emit started(false);
        return;
    }

    DOWN_LOG(INFO) << "Network is accessible, performing download request";
    // signals should take care of calling deleteLater on the
    // NetworkReply object
    _reply = _requestFactory->get(buildRequest());
    _reply->setReadBufferSize(throttle());

    connectToReplySignals();
    DOWN_LOG(INFO) << "EMIT started(true)";
    _downloading = true;
    emit started(true);
}

qulonglong
FileDownload::progress() {
    return (_currentData == nullptr) ? 0 : _currentData->size();
}

qulonglong
FileDownload::totalSize() {
    return _totalSize;
}

void
FileDownload::setThrottle(qulonglong speed) {
    TRACE << _url;
    Download::setThrottle(speed);
    if (_reply != nullptr)
        _reply->setReadBufferSize(speed);
}

void
FileDownload::setDestinationDir(const QString& path) {
    // we have to perform several checks to ensure the integrity
    // of the download
    // 1. Trust apparmor that it will bock the call of this method
    //    from not confined apps
    // 2. Ensure that the path is an absolute path.
    // 3. Ensure that the path does exist.
    // 4. Ensure that the path is a dir.
    // 5. Ensure that the download was not started
    QFileInfo info(path);
    if (!info.isAbsolute()) {
        DOWN_LOG(WARNING) << "Trying to set destination dir to '"
            << path << "' when the path is not absolute.";
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::InvalidArgs,
                "The destination dir must be an absolute path.");
        }
        return;
    }

    if (!info.exists()) {
        DOWN_LOG(WARNING) << "Trying to set destination dir to '"
            << path << "' when the path does not exists.";
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::InvalidArgs,
                "The destination dir must be already present in the system.");
        }
        return;
    }

    if (!info.isDir()) {
        DOWN_LOG(WARNING) << "Trying to set destination dir to '"
            << path << "' when the path is not a dir.";
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::InvalidArgs,
                "The destination dir must be a dir and a file was found.");
        }
        return;
    }

    if (state() == Download::IDLE) {
        // calculate the new path before we unlock the old one
        auto desiredPath = path;
        if (!desiredPath.endsWith(QDir::separator())) {
            desiredPath += QDir::separator();
        }
        desiredPath += _basename;
        desiredPath = _fileNameMutex->lockFileName(desiredPath);
        _tempFilePath = desiredPath + TEMP_EXTENSION;
        _fileNameMutex->unlockFileName(_filePath);
        _filePath = desiredPath;
    } else {
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::NotSupported,
                "The path cannot be changed in a started download.");
        }
    }
}

void
FileDownload::setHeaders(StringMap headers) {
    TRACE << "Setting headers too " << headers;
    // header can only be set BEFORE the download was ever started
    if (state() == Download::IDLE) {
        Download::setHeaders(headers);
    } else {
        DOWN_LOG(WARNING) << "Trying to set headers for already started download.";
        if (calledFromDBus()) {
            sendErrorReply(QDBusError::NotSupported,
                "The path cannot be changed in a started download.");
        }
    }
}

void
FileDownload::onDownloadProgress(qint64 currentProgress, qint64 bytesTotal) {
    TRACE << _url << currentProgress << bytesTotal;

    // write the current info we have, just in case we are killed in the
    // middle of the download
    _currentData->write(_reply->readAll());
    if (!flushFile()) {
        return;
    }
    qulonglong received = _currentData->size();

    if (bytesTotal == -1) {
        // we do not know the size of the download, simply return
        // the same for received and for total
        emit Download::progress(received, received);
        return;
    } else {
        if (_totalSize == 0) {
            qlonglong uBytestTotal = bytesTotal;
            // bytesTotal is different when we have resumed because we
            // are not counting the size that  we already downloaded,
            // therefore we only do this once
            // update the metadata
            _totalSize = uBytestTotal;
        }
        emit Download::progress(received, _totalSize);
        return;
    }
}

void
FileDownload::onError(QNetworkReply::NetworkError code) {
    DOWN_LOG(ERROR) << _url << " ERROR:" << ":" << code;
    _downloading = false;
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

void
FileDownload::onRedirect(QUrl redirect) {
    DOWN_LOG(INFO) << "Following redirect to" << redirect;
    // update the _url value and perform a second request to try and get the data
    if (_visitedUrls.contains(redirect)) {
        // we are in a loop!!! we have to raise an error about this.
        DOWN_LOG(WARNING) << "Redirect loop found";
        NetworkErrorStruct err(QNetworkReply::ContentNotFoundError);
        emit networkError(err);
        emitError(NETWORK_ERROR);
        return;
    }
    _visitedUrls.append(_url);
    _url = redirect;

    // clean the reply
    disconnectFromReplySignals();
    _reply->deleteLater();
    _reply = nullptr;

    // clean the local file without unlocking the file the reason for
    // this is that the file pat is going to be reused to store the
    // data from the redirect
    cleanUpCurrentData();

    // perform again the request but do not emit started signal
    _currentData = FileManager::instance()->createFile(_tempFilePath);
    bool canWrite = _currentData->open(QIODevice::ReadWrite | QFile::Append);

    if (!canWrite) {
        emitError(FILE_SYSTEM_ERROR);
        return;
    }
    _reply = _requestFactory->get(buildRequest());
    _reply->setReadBufferSize(throttle());

    connectToReplySignals();
}

void
FileDownload::onDownloadCompleted() {
    TRACE << _url;

    // check if we have the content-type header, if we do we are going to change the
    // file path that will be used by the download, do not do it if the app is
    // unconfined
    if ((_reply->hasRawHeader(CONTENT_DISPOSITION) && (
            isConfined() || !_metadata.contains(Metadata::LOCAL_PATH_KEY)))) {
        QString contentDisposition = _reply->rawHeader(CONTENT_DISPOSITION);
        DOWN_LOG(INFO) << "Content-Disposition header" << contentDisposition;

        if (contentDisposition.contains("filename")) {
            auto serverName = filenameFromHTTPContentDisposition(contentDisposition);
            DOWN_LOG(INFO) << "Server name " << serverName;

            if (!serverName.isEmpty()) {
                QFileInfo fiContentDisposition(serverName);
                auto filename = fiContentDisposition.fileName();
                // replace the filename of the current _filePath with the new one
                QFileInfo fiFilePath(_filePath);
                auto currentFileName = fiFilePath.fileName();
                auto newPath = _filePath.replace(currentFileName, filename);

                // unlock the old path and lock the new one
                _fileNameMutex->unlockFileName(_filePath);
                _filePath = _fileNameMutex->lockFileName(newPath);
                DOWN_LOG(INFO) << "Content disposition based file path is '" << serverName << "'";
            }
        }
    }

    // if the hash is present we check it
    if (!_hash.isEmpty()) {
        emit processing(filePath());
        _currentData->reset();
        auto hashFactory = CryptographicHashFactory::instance();
        QScopedPointer<CryptographicHash> hash(
            hashFactory->createCryptographicHash(_algo, this));
        // addData is smart enough to not load the entire file in memory
        hash->addData(_currentData->device());
        QString fileSig = QString(hash->result().toHex());

        if (fileSig != _hash) {
            DOWN_LOG(ERROR) << HASH_ERROR << fileSig << "!=" << _hash;
            emitError(HASH_ERROR);
            return;
        }
    }

    // there are two possible cases, the first, we do not have the metadata
    // info to execute a command once the download was finished and that
    // means we are done here else we execute the command AND raise the
    // finish signals once the command was done (or an error occurred in
    // the command execution.
    if (_metadata.contains(Metadata::COMMAND_KEY)) {
        // just emit processing if we DO NOT have a hash because else we
        // already emitted it.
        if (_hash.isEmpty()) {
            emit processing(filePath());
        }
        // toStringList will return an empty list if it cannot be converted
        QStringList commandData =
            _metadata[Metadata::COMMAND_KEY].toStringList();
        if (commandData.count() == 0) {
            DOWN_LOG(ERROR) << "COMMAND DATA MISSING";
            emitError(COMMAND_ERROR);
            return;
        } else {
            // first item of the string list is the command
            // the rest is the arguments
            DOWN_LOG(INFO) << "Renaming '" << _tempFilePath << "'"
                << "' to '" << _filePath << "'";

            auto fileMan = FileManager::instance();
            if (fileMan->exists(_tempFilePath)) {
                LOG(INFO) << "Renaming: '" << _tempFilePath << "' to '"
                    << _filePath << "'";
                fileMan->rename(_tempFilePath, _filePath);
            }

            QString command = commandData.at(0);
            commandData.removeAt(0);
            QStringList args;

            foreach(const QString& arg, commandData) {
                if (arg == Metadata::COMMAND_FILE_KEY)
                    args << filePath();
                else
                    args << arg;
            }

            Process* postDownloadProcess =
                ProcessFactory::instance()->createProcess();

            // connect to signals so that we can tell the clients that
            // the operation succeed

            CHECK(connect(postDownloadProcess, &Process::finished,
                this, &FileDownload::onProcessFinished))
                    << "Could not connect to signal";
            CHECK(connect(postDownloadProcess, &Process::error,
                this, &FileDownload::onProcessError))
                    << "Could not connect to signal";

            DOWN_LOG(INFO) << "Executing" << command << args;
            postDownloadProcess->start(command, args);
            return;
        }
    } else {
        emitFinished();
    }

    // clean the reply
    _reply->deleteLater();
    _reply = nullptr;
}

void
FileDownload::onFinished() {
    TRACE << _url;
    // the reply has finished but the resource might have been moved
    // and we must do a new request
    auto redirectVar = _reply->attribute(
        QNetworkRequest::RedirectionTargetAttribute);

    if(redirectVar.isValid()) {
        auto redirect = redirectVar.toUrl();
        if(!redirect.isEmpty() && redirect != _url) {
            onRedirect(redirect);
            return;
        }
    }
    onDownloadCompleted();
}

void
FileDownload::onSslErrors(const QList<QSslError>& errors) {
    TRACE << errors;
    if (!_reply->canIgnoreSslErrors(errors)) {
        _downloading = false;
        emitError(SSL_ERROR);
    }
}

void
FileDownload::onProcessError(QProcess::ProcessError error) {
    auto p = qobject_cast<Process*>(sender());
    auto standardOut = p->readAllStandardOutput();
    auto standardErr = p->readAllStandardError();
    DOWN_LOG(ERROR) << "Error " << error << "executing"
        << p->program() << "with args" << p->arguments()
        << "Stdout:" << standardOut << "Stderr:" << standardErr;
    p->deleteLater();
    ProcessErrorStruct err(error, 0, standardOut, standardErr);
    emit processError(err);
    emitError(COMMAND_ERROR);
}

void
FileDownload::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    TRACE << exitCode << exitStatus;
    auto p = qobject_cast<Process*>(sender());
    if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
        // remove the file since we are done with it
        cleanUpCurrentData();
        emitFinished();
        // remove the file because that is the contract that we have with
        // the clients
        auto fileMan = FileManager::instance();

        if (fileMan->exists(_tempFilePath)) {
            LOG(INFO) << "Removing '" << _tempFilePath << "'";
            fileMan->remove(_tempFilePath);
        }

        if (fileMan->exists(_filePath)) {
            LOG(INFO) << "Removing '" << _filePath << "'";
            fileMan->remove(_filePath);
        }
    } else {
        auto standardOut = p->readAllStandardOutput();
        auto standardErr = p->readAllStandardError();
        ProcessErrorStruct err(exitStatus, "ErrorInProcess", exitCode,
            standardOut, standardErr);
        emit processError(err);
        emitError(COMMAND_ERROR);
    }
    p->deleteLater();
}

void
FileDownload::onOnlineStateChanged(bool online) {
    TRACE << online;
    _connected = online;
    // if we are downloading and the status is correct let's call
    // the method again, else do nothing
    if (_connected && _downloading) {
        Download::State currentState = state();
        if (currentState == Download::START || currentState == Download::RESUME) {
            resumeTransfer();
        }
    }

    // if no longer online yet we have a reply (that is, we are trying
    // to get data from the missing connection) we pause
    if (!_connected && _reply != nullptr) {
        pauseTransfer();
        // set it to be downloading even when pause download sets it
        // to false
        _downloading = true;
    }
}

void
FileDownload::init() {
    _requestFactory = RequestFactory::instance();
    _fileNameMutex = FileNameMutex::instance();
    SystemNetworkInfo* networkInfo = SystemNetworkInfo::instance();
    _connected = networkInfo->isOnline();
    _downloading = false;

    // online unconfined applications are allows to set the click package name
    // in the metadata
    if (isConfined() && _metadata.contains(Metadata::CLICK_PACKAGE_KEY)) {
        _metadata.remove(Metadata::CLICK_PACKAGE_KEY);
    }

    // connect to the network changed signals
    CHECK(connect(networkInfo, &SystemNetworkInfo::onlineStateChanged,
        this, &FileDownload::onOnlineStateChanged))
            << "Could not connect to signal";

    initFileNames();

    // ensure that the download is valid
    if (!_url.isValid()) {
        setIsValid(false);
        setLastError(QString("Invalid URL: '%1'").arg(_url.toString()));
    }
}

void
FileDownload::connectToReplySignals() {
    if (_reply != nullptr) {
        CHECK(connect(_reply, &NetworkReply::downloadProgress,
            this, &FileDownload::onDownloadProgress))
                << "Could not connect to signal";
        CHECK(connect(_reply, &NetworkReply::error,
            this, &FileDownload::onError))
                << "Could not connect to signal";
        CHECK(connect(_reply, &NetworkReply::finished,
            this, &FileDownload::onFinished))
                << "Could not connect to signal";
        CHECK(connect(_reply, &NetworkReply::sslErrors,
            this, &FileDownload::onSslErrors))
                << "Could not connect to signal";
    }
}

void
FileDownload::disconnectFromReplySignals() {
    if (_reply != nullptr) {
        disconnect(_reply, &NetworkReply::downloadProgress,
            this, &FileDownload::onDownloadProgress);
        disconnect(_reply, &NetworkReply::error,
            this, &FileDownload::onError);
        disconnect(_reply, &NetworkReply::finished,
            this, &FileDownload::onFinished);
        disconnect(_reply, &NetworkReply::sslErrors,
            this, &FileDownload::onSslErrors);
    }
}

bool
FileDownload::flushFile() {
    auto flushed  = _currentData->flush();
    if (!flushed) {
        auto err = _currentData->error();
        DOWN_LOG(ERROR) << "Could not write that in the file system" << err;
        emitError(QString(FILE_SYSTEM_ERROR).arg(err));
    }
    return flushed;
}

void
FileDownload::initFileNames() {
    // the mutex will ensure that we do not have race conditions about
    // the file names in the download manager
    QString path = _url.path();
    _basename = QFileInfo(path).fileName();

    if (_basename.isEmpty()) {
        QScopedPointer<UuidFactory> uuidFactory(new UuidFactory());
        _basename = UuidUtils::getDBusString(uuidFactory->createUuid());
    }

    if (!isConfined() && _metadata.contains(Metadata::LOCAL_PATH_KEY)) {
        _filePath = _metadata[Metadata::LOCAL_PATH_KEY].toString();
        _tempFilePath = _fileNameMutex->lockFileName(
            _filePath + TEMP_EXTENSION);

        // in this case and because the app is not confined we are
        // going to check if the file exists, if it does we will
        // raise an error
        if (QFile::exists(_filePath)) {
            setIsValid(false);
            setLastError(QString("File already exists at: '%1'").arg(
                _filePath));
        }
    } else {
        auto desiredPath = rootPath() + QDir::separator() + _basename;
        _filePath = _fileNameMutex->lockFileName(desiredPath);
        _tempFilePath = _filePath + TEMP_EXTENSION;
    }
}

void
FileDownload::emitFinished() {
    auto fileMan = FileManager::instance();


    if (fileMan->exists(_tempFilePath)) {
        DOWN_LOG(INFO) << "Rename '" << _tempFilePath << "' to '"
            << _filePath << "'";
        QFile tempFile(_tempFilePath);
        auto r = tempFile.rename(_filePath);
        if (!r) {
            DOWN_LOG(WARNING) << "Could not rename '" << _tempFilePath << "' to '"
                << _filePath << "' due to " << tempFile.errorString();
        }
    }

    setState(Download::FINISH);
    unlockFilePath();

    DOWN_LOG(INFO) << "EMIT finished" << filePath();
    emit finished(_filePath);
}

void
FileDownload::unlockFilePath() {
    if (!isConfined() && _metadata.contains(Metadata::LOCAL_PATH_KEY)) {
        _fileNameMutex->unlockFileName(_tempFilePath);
    } else {
        _fileNameMutex->unlockFileName(_filePath);
    }
}

QString
FileDownload::filenameFromHTTPContentDisposition(const QString& value) {
    auto keyValuePairs = value.split(';');

    foreach(const QString& valuePair, keyValuePairs) {
        int valueStartPos = valuePair.indexOf('=');

        if (valueStartPos < 0)
            continue;

        auto pair = valuePair.split('=');
        if (pair.size() != 2 || pair[0].isEmpty() || pair[0].simplified() != "filename")
            continue;

        auto value = pair[1].replace("\"", "") // remove ""
            .replace("'", "") // remove '
            .simplified();  // remove white spaces
        return value;
    }

    return QString();
}

void
FileDownload::cleanUpCurrentData() {
    bool success = true;
    QFile::FileError error = QFile::NoError;
    if (_currentData != nullptr) {
        success = _currentData->remove();

        if (!success)
            error = _currentData->error();

        _currentData->deleteLater();
        _currentData = nullptr;
    } else {
        QScopedPointer<QFile> tempFile(new QFile(_tempFilePath));
        success = tempFile->remove();
        if (!success)
            error = tempFile->error();
    }

    if (!success)
        DOWN_LOG(WARNING) << "Error " << error <<
            "removing file with path" << _filePath;
}

QNetworkRequest
FileDownload::buildRequest() {
    QNetworkRequest request = QNetworkRequest(_url);
    QMap<QString, QString> headersMap = headers();
    foreach(const QString& header, headersMap.keys()) {
        QString data = headersMap[header];
        if (header.toLower() == "range"
                || header.toLower() == "accept-encoding")
            // do not add the header
            continue;
        request.setRawHeader(header.toUtf8(), data.toUtf8());
    }
    // very important we must ensure that we do not decompress any download
    // else we will have an error in the checksum for example #1224678
    request.setRawHeader("Accept-Encoding", "identity");
    return request;
}

void
FileDownload::emitError(const QString& error) {
    TRACE << error;
    disconnectFromReplySignals();
    _reply->deleteLater();
    _reply = nullptr;
    cleanUpCurrentData();
    // let other downloads use the same file name
    unlockFilePath();
    Download::emitError(error);
}

}  // Daemon

}  // DownloadManager

}  // Ubuntu
