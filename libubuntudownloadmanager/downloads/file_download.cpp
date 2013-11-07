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

#include <QBuffer>
#include <QCryptographicHash>
#include <QDir>
#include <QDebug>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QSslError>
#include "downloads/file_download.h"
#include "system/hash_algorithm.h"
#include "system/logger.h"
#include "system/network_reply.h"


#define DATA_FILE_NAME "data.download"
#define METADATA_FILE_NAME "metadata"
#define METADATA_COMMAND_KEY "post-download-command"
#define METADATA_COMMAND_FILE_KEY "$file"

namespace Ubuntu {

namespace DownloadManager {


/**
 * PRIVATE IMPLEMENATION
 */

class FileDownloadPrivate {
    Q_DECLARE_PUBLIC(FileDownload)

 public:
    FileDownloadPrivate(const QUrl& url,
                        QSharedPointer<RequestFactory> nam,
                        QSharedPointer<ProcessFactory> processFactory,
                        FileDownload* parent)
        : _totalSize(0),
          _url(url),
          _hash(""),
          _algo(QCryptographicHash::Md5),
          _requestFactory(nam),
          _processFactory(processFactory),
          q_ptr(parent) {
        init();
    }

    FileDownloadPrivate(const QUrl& url,
                        const QString& hash,
                        const QString& algo,
                        QSharedPointer<RequestFactory> nam,
                        QSharedPointer<ProcessFactory> processFactory,
                        FileDownload* parent)

        : _totalSize(0),
          _url(url),
          _hash(hash),
          _requestFactory(nam),
          _processFactory(processFactory),
          q_ptr(parent) {
        Q_Q(FileDownload);
        init();
        _algo = HashAlgorithm::getHashAlgo(algo);
        // check that the algorithm is correct if the hash is not emtpy
        if (!_hash.isEmpty() && !HashAlgorithm::isValidAlgo(algo)) {
            q->setIsValid(false);
            q->setLastError(QString("Invalid hash algorithm: '%1'").arg(algo));
        }
    }

    ~FileDownloadPrivate() {
        if (_currentData != NULL) {
            _currentData->close();
            delete _currentData;
        }
        if (_reply != NULL)
            delete _reply;
    }

    QUrl url() const {
        return _url;
    }

    QString filePath() {
        return _filePath;
    }

    QString hash() const {
        return _hash;
    }

    QCryptographicHash::Algorithm hashAlgorithm() const {
        return _algo;
    }

    void cancelDownload() {
        Q_Q(FileDownload);
        TRACE << _url;

        if (_reply != NULL) {
            // disconnect so that we do not get useless signals
            // and remove the reply
            disconnectFromReplySignals();
            _reply->abort();
            _reply->deleteLater();
            _reply = NULL;
        }

        // remove current data and metadata
        cleanUpCurrentData();
        _downloading = false;
        emit q->canceled(true);
    }

    void pauseDownload() {
        Q_Q(FileDownload);
        TRACE << _url;

        if (_reply == NULL) {
            // cannot pause because is not running
            qDebug() << "Cannot pause download because reply is NULL";
            qDebug() << "EMIT paused(false)";
            emit q->paused(false);
            return;
        }

        qDebug() << "Pausing download" << _url;;
        // we need to disconnect the signals to ensure that they are not
        // emitted due to the operation we are going to perform. We read
        // the data in the reply and store it in a file
        disconnectFromReplySignals();

        // do abort before reading
        _reply->abort();
        _currentData->write(_reply->readAll());
        _reply->deleteLater();
        _reply = NULL;
        qDebug() << "EMIT paused(true)";
        _downloading = false;
        emit q->paused(true);
    }

    void resumeDownload() {
        Q_Q(FileDownload);
        qDebug() << __PRETTY_FUNCTION__ << _url;

        if (_reply != NULL) {
            // cannot resume because it is already running
            qDebug() << "Cannot resume download because reply != NULL";
            qDebug() << "EMIT resumed(false)";
            emit q->resumed(false);
            return;
        }

        qDebug() << "Resuming download.";
        QNetworkRequest request = buildRequest();

        // overrides the range header, we do not let clients set the range!!!
        qint64 currentDataSize = _currentData->size();
        QByteArray rangeHeaderValue = "bytes=" +
            QByteArray::number(currentDataSize) + "-";
        request.setRawHeader("Range", rangeHeaderValue);

        _reply = _requestFactory->get(request);
        _reply->setReadBufferSize(q->throttle());

        connectToReplySignals();

        qDebug() << "EMIT resumed(true)";
        _downloading = true;
        emit q->resumed(true);
    }

    void startDownload() {
        Q_Q(FileDownload);
        TRACE << _url;

        if (_reply != NULL) {
            // the download was already started, lets say that we did it
            qDebug() << "Cannot start download because reply != NULL";
            qDebug() << "EMIT started(false)";
            emit q->started(true);
            return;
        }

        // create file that will be used to mantain the state of the
        // download when resumed.
        _currentData = new QFile(_filePath);
        _currentData->open(QIODevice::ReadWrite | QFile::Append);

        qDebug() << "Network is accessible, performing download request";
        // signals should take care of calling deleteLater on the
        // NetworkReply object
        _reply = _requestFactory->get(buildRequest());
        _reply->setReadBufferSize(q->throttle());

        connectToReplySignals();
        qDebug() << "EMIT started(true)";
        _downloading = true;
        emit q->started(true);
    }

    qulonglong progress() {
        return (_currentData == NULL)?0:_currentData->size();
    }

    qulonglong totalSize() {
        return _totalSize;
    }

    void setThrottle(qulonglong speed) {
        TRACE << _url;

        if (_reply != NULL)
            _reply->setReadBufferSize(speed);
    }

    // slots executed to keep track of the network reply
    void onDownloadProgress(qint64 progress, qint64 bytesTotal) {
        Q_Q(FileDownload);
        TRACE << _url << progress << bytesTotal;

        // write the current info we have, just in case we are killed in the
        // middle of the download
        _currentData->write(_reply->readAll());
        _currentData->flush();
        qulonglong received = _currentData->size();

        if (bytesTotal == -1) {
            // we do not know the size of the download, simply return
            // the same for received and for total
            emit reinterpret_cast<Download*>(q)->progress(received, received);
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
            emit reinterpret_cast<Download*>(q)->progress(received, _totalSize);
            return;
        }
    }

    void onError(QNetworkReply::NetworkError code) {
        qCritical() << _url << "ERROR:" << ":" << code;
        _downloading = false;
        emitError("NETWORK ERROR");
    }

    void onFinished() {
        Q_Q(FileDownload);
        TRACE << _url;

        // if the hash is present we check it
        if (!_hash.isEmpty()) {
            _currentData->reset();
            QByteArray data = _currentData->readAll();
            // do calculate the hash of the file so far and ensure that
            // they are the same
            QString fileSig = QString(
                QCryptographicHash::hash(data, _algo).toHex());
            if (fileSig != _hash) {
                qCritical() << "HASH ERROR:" << fileSig << "!=" << _hash;
                emitError("HASH ERROR");
                return;
            }
        }

        // there are two possible cases, the first, we do not have the metadata
        // info to execute a command once the download was finished and that
        // means we are done here else we execute the command AND raise the
        // finish signals once the command was done (or an error ocurred in
        // the command execution.
        if (q->metadata().contains(METADATA_COMMAND_KEY)) {
            // toStringList will return an empty list if it cannot be converted
            QStringList commandData =
                q->metadata()[METADATA_COMMAND_KEY].toStringList();
            if (commandData.count() == 0) {
                // raise error, command metadata was passed without the commnad
                qCritical() << "COMMAND DATA MISSING";
                emitError("COMMAND ERROR");
                return;
            } else {
                // first item of the string list is the commnad
                // the rest is the arguments
                QString command = commandData.at(0);
                commandData.removeAt(0);
                QStringList args;

                foreach(const QString& arg, commandData) {
                    if (arg == METADATA_COMMAND_FILE_KEY)
                        args << filePath();
                    else
                        args << arg;
                }

                Process* postDownloadProcess = _processFactory->createProcess();

                // connect to signals so that we can tell the clients that
                // the operation succeed

                q->connect(postDownloadProcess,
                           SIGNAL(finished(int, QProcess::ExitStatus)), q,
                           SLOT(onProcessFinished(int, QProcess::ExitStatus)));
                q->connect(postDownloadProcess,
                           SIGNAL(error(QProcess::ProcessError)), q,
                           SLOT(onProcessError(QProcess::ProcessError)));

                qDebug() << "Executing" << command << args;
                postDownloadProcess->start(command, args);
                emit q->processing(filePath());
                return;
            }
        } else {
            q->setState(Download::FINISH);
            qDebug() << "EMIT finished" << filePath();
            emit q->finished(filePath());
        }

        // clean the reply
        _reply->deleteLater();
        _reply = NULL;
    }

    void onSslErrors(const QList<QSslError>& errors) {
        TRACE << errors;
        Q_UNUSED(errors);
        if (!_reply->canIgnoreSslErrors(errors)) {
            _downloading = false;
            emitError("SSL ERROR");
        }
    }

    // slots executed to keep track of the post download process
    void onProcessError(QProcess::ProcessError error) {
        TRACE << error;
        Q_UNUSED(error);
        emitError("COMMAND ERROR");
    }

    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
        TRACE << exitCode << exitStatus;
        Q_Q(FileDownload);
        if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
            // remove the file since we are done with it
            bool success = QFile::remove(_filePath);
            if (!success)
                qWarning() << "Error removing" << _filePath;
            q->setState(Download::FINISH);
            qDebug() << "EMIT finished" << filePath();
            emit q->finished(filePath());
        } else {
            emitError("COMMAND ERROR");
        }
    }

    void onOnlineStateChanged(bool online) {
        TRACE << online;
        Q_Q(FileDownload);
        _connected = online;
        // if we are downloading and the status is correct let's call
        // the method again, else do nothing
        if (_connected && _downloading) {
            Download::State state = q->state();
            if (state == Download::START || state == Download::RESUME) {
                resumeDownload();
            }
        }

        // if no longer online yet we have a reply (that is, we are trying
        // to get data from the missing connection) we pause
        if (!_connected && _reply != NULL) {
            pauseDownload();
            // set it to be downloading even when pause download sets it
            // to false
            _downloading = true;
        }
    }

 private:
    void init() {
        Q_Q(FileDownload);

        _connected = q->networkInfo()->isOnline();
        _downloading = false;

        // connect to the network changed signals
        q->connect(q->networkInfo().data(),
            SIGNAL(onlineStateChanged(bool)), q,
            SLOT(onOnlineStateChanged(bool)));

        _filePath = saveFileName();
        _reply = NULL;
        _currentData = NULL;

        // ensure that the download is valid
        if (!_url.isValid()) {
            q->setIsValid(false);
            q->setLastError(QString("Invalid URL: '%1'").arg(_url.toString()));
        }
    }

    void connectToReplySignals() {
        Q_Q(FileDownload);
        if (_reply != NULL) {
            q->connect(_reply, SIGNAL(downloadProgress(qint64, qint64)),
                q, SLOT(onDownloadProgress(qint64, qint64)));
            q->connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)),
                q, SLOT(onError(QNetworkReply::NetworkError)));
            q->connect(_reply, SIGNAL(finished()),
                q, SLOT(onFinished()));
            q->connect(_reply, SIGNAL(sslErrors(const QList<QSslError>&)),
                q, SLOT(onSslErrors(const QList<QSslError>&)));
        }
    }

    void disconnectFromReplySignals() {
        Q_Q(FileDownload);
        if (_reply != NULL) {
            q->disconnect(_reply, SIGNAL(downloadProgress(qint64, qint64)),
                q, SLOT(onDownloadProgress(qint64, qint64)));
            q->disconnect(_reply, SIGNAL(error(QNetworkReply::NetworkError)),
                q, SLOT(onError(QNetworkReply::NetworkError)));
            q->disconnect(_reply, SIGNAL(finished()),
                q, SLOT(onFinished()));
            q->disconnect(_reply, SIGNAL(sslErrors(const QList<QSslError>&)),
                q, SLOT(onSslErrors(const QList<QSslError>&)));
        }
    }

    QString saveFileName() {
        Q_Q(FileDownload);

        QString path = _url.path();
        QString basename = QFileInfo(path).fileName();

        if (basename.isEmpty())
            basename = DATA_FILE_NAME;

        QVariantMap metadata = q->metadata();
        QString finalPath;

        if (!q->isConfined() && metadata.contains(LOCAL_PATH_KEY)) {
            finalPath = metadata[LOCAL_PATH_KEY].toString();

            // in this case and because the app is not confined we are
            // going to check if the file exists, if it does we will
            // raise an error
            if (QFile::exists(finalPath)) {
                q->setIsValid(false);
                q->setLastError(QString("File already exists at: '%1'").arg(
                    finalPath));
            }
        } else {
            finalPath = q->rootPath() + QDir::separator() + basename;

            // check if the file exists, if it does lets append the uuid to it
            if (QFile::exists(finalPath)) {
                finalPath += q->downloadId();
            }
        }

        return finalPath;
    }

    void cleanUpCurrentData() {
        bool success;
        QString fileName;
        if (_currentData) {
            // delete the current data, we did cancel.
            fileName = _currentData->fileName();
            success = _currentData->remove();
            _currentData->deleteLater();
            _currentData = NULL;
            if (!success)
                qWarning() << "Error removing" << fileName;
        }
        success = QFile::remove(_filePath);
        if (!success)
            qWarning() << "Error removing" << _filePath;
    }

    QNetworkRequest buildRequest() {
        Q_Q(FileDownload);
        QNetworkRequest request = QNetworkRequest(_url);
        QMap<QString, QString> headers = q->headers();
        foreach(const QString& header, headers.keys()) {
            QString data = headers[header];
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

    void emitError(const QString& error) {
        TRACE << error;
        Q_Q(FileDownload);
        disconnectFromReplySignals();
        _reply->deleteLater();
        _reply = NULL;
        cleanUpCurrentData();
        q->emitError(error);
    }

 private:
    bool _downloading = false;
    bool _connected = false;
    qulonglong _totalSize = 0;
    QUrl _url;
    QString _filePath;
    QString _hash;
    QCryptographicHash::Algorithm _algo;
    NetworkReply* _reply;
    QFile* _currentData;
    QSharedPointer<RequestFactory> _requestFactory;
    QSharedPointer<ProcessFactory> _processFactory;
    FileDownload* q_ptr;
};

/**
 * PUBLIC IMPLEMENTATION
 */

FileDownload::FileDownload(const QString& id,
                   const QString& path,
                   bool isConfined,
                   const QString& rootPath,
                   const QUrl& url,
                   const QVariantMap& metadata,
                   const QMap<QString, QString>& headers,
                   QSharedPointer<SystemNetworkInfo> networkInfo,
                   QSharedPointer<RequestFactory> nam,
                   QSharedPointer<ProcessFactory> processFactory,
                   QObject* parent)
    : Download(id, path, isConfined, rootPath, metadata, headers, networkInfo,
            parent),
      d_ptr(new FileDownloadPrivate(url, nam, processFactory, this)) {
}

FileDownload::FileDownload(const QString& id,
                   const QString& path,
                   bool isConfined,
                   const QString& rootPath,
                   const QUrl& url,
                   const QString& hash,
                   const QString& algo,
                   const QVariantMap& metadata,
                   const QMap<QString, QString> &headers,
                   QSharedPointer<SystemNetworkInfo> networkInfo,
                   QSharedPointer<RequestFactory> nam,
                   QSharedPointer<ProcessFactory> processFactory,
                   QObject* parent)
    : Download(id, path, isConfined, rootPath, metadata, headers, networkInfo,
            parent),
      d_ptr(new FileDownloadPrivate(url, hash, algo, nam,
            processFactory, this)) {
}

QUrl
FileDownload::url() {
    Q_D(FileDownload);
    return d->url();
}

QString
FileDownload::filePath() {
    Q_D(FileDownload);
    return d->filePath();
}

QString
FileDownload::hash() {
    Q_D(FileDownload);
    return d->hash();
}

QCryptographicHash::Algorithm
FileDownload::hashAlgorithm() {
    Q_D(FileDownload);
    return d->hashAlgorithm();
}

void
FileDownload::cancelDownload() {
    Q_D(FileDownload);
    d->cancelDownload();
}

void
FileDownload::pauseDownload() {
    Q_D(FileDownload);
    d->pauseDownload();
}

void
FileDownload::resumeDownload() {
    Q_D(FileDownload);
    d->resumeDownload();
}

void
FileDownload::startDownload() {
    Q_D(FileDownload);
    d->startDownload();
}

qulonglong
FileDownload::progress() {
    Q_D(FileDownload);
    return d->progress();
}

qulonglong
FileDownload::totalSize() {
    Q_D(FileDownload);
    return d->totalSize();
}

void
FileDownload::setThrottle(qulonglong speed) {
    Q_D(FileDownload);
    Download::setThrottle(speed);
    d->setThrottle(speed);
}

}  // DownloadManager

}  // Ubuntu
#include "moc_file_download.cpp"
