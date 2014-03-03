/*
 * copyright 2013-2014 canonical ltd.
 *
 * this library is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the gnu lesser general public
 * license as published by the free software foundation.
 *
 * this program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * merchantability or fitness for a particular purpose.  see the gnu
 * general public license for more details.
 *
 * you should have received a copy of the gnu lesser general public
 * license along with this library; if not, write to the
 * free software foundation, inc., 51 franklin street, fifth floor,
 * boston, ma 02110-1301, usa.
 */

#include <ubuntu/download_manager/metadata.h>
#include <ubuntu/download_manager/system/hash_algorithm.h>
#include "ubuntu/system/logger.h"
#include "ubuntu/system/uuid_factory.h"
#include "download_adaptor.h"
#include "file_download.h"
#include "group_download.h"

#define GROUP_LOG(LEVEL) LOG(LEVEL) << "Group Download{" << objectName() << "}"

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

GroupDownload::GroupDownload(const QString& id,
                  const QString& path,
                  bool isConfined,
                  const QString& rootPath,
                  QList<GroupDownloadStruct> downloads,
                  const QString& algo,
                  bool isGSMDownloadAllowed,
                  const QVariantMap& metadata,
                  const QMap<QString, QString>& headers,
                  Factory* downFactory,
                  QObject* parent)
    : Download(id, path, isConfined, rootPath, metadata, headers,
            parent),
      _downloads(),
      _finishedDownloads(),
      _downloadsProgress(),
      _downFactory(downFactory) {
    init(downloads, algo, isGSMDownloadAllowed);
}

GroupDownload::~GroupDownload() {
    qDeleteAll(_downloads);
}

void
GroupDownload::connectToDownloadSignals(FileDownload* singleDownload) {
    connect(singleDownload, static_cast<void(Download::*)
            (qulonglong, qulonglong)>(&Download::progress),
                this, &GroupDownload::onProgress);
    connect(singleDownload, &FileDownload::finished,
        this, &GroupDownload::onFinished);
    connect(singleDownload, &FileDownload::processing,
        this, &GroupDownload::processing);

    // connect to the error signals
    connect(singleDownload, &Download::error,
        this, &GroupDownload::onError);
    connect(singleDownload, &FileDownload::authError,
        this, &GroupDownload::onAuthError);
    connect(singleDownload, &FileDownload::httpError,
        this, &GroupDownload::onHttpError);
    connect(singleDownload, &FileDownload::networkError,
        this, &GroupDownload::onNetworkError);
    connect(singleDownload, &FileDownload::processError,
        this, &GroupDownload::onProcessError);
}

void
GroupDownload::init(QList<GroupDownloadStruct> downloads,
              const QString& algo,
              bool isGSMDownloadAllowed) {
    _fileManager = FileManager::instance();
    QVariantMap metadataMap = metadata();
    QMap<QString, QString> headersMap = headers();
    QStringList localPaths;

    // build downloads and add them to the q, it will take care of
    // starting them etc..
    foreach(GroupDownloadStruct download, downloads) {
        QUrl url(download.getUrl());
        QString hash = download.getHash();

        FileDownload* singleDownload;
        QVariantMap downloadMetadata = QVariantMap(metadataMap);
        downloadMetadata[Metadata::LOCAL_PATH_KEY] = download.getLocalFile();

        if (hash.isEmpty()) {
            singleDownload = qobject_cast<FileDownload*>(
                _downFactory->createDownloadForGroup(isConfined(),
                    rootPath(), url, downloadMetadata, headersMap));
        } else {

            if (!HashAlgorithm::isValidAlgo(algo)) {
                setIsValid(false);
                setLastError(QString("Invalid hash algorithm: '%1'").arg(algo));
            }

            singleDownload = qobject_cast<FileDownload*>(
                _downFactory->createDownloadForGroup(isConfined(),
                    rootPath(), url, hash, algo, downloadMetadata,
                    headersMap));
        }
        singleDownload->setParent(this);

        // ensure that the local path is not used by any other download
        // in this group.
        QString localFilePath = singleDownload->filePath();
        if (localPaths.contains(localFilePath)) {
            setIsValid(false);
            setLastError("Duplicated local path passed: " + localFilePath);
            break;
        } else {
            localPaths.append(localFilePath);
        }

        // check that the download is valid, if it is not set to invalid
        // and use the last error from the download
        if (!singleDownload->isValid()) {
            setIsValid(false);
            setLastError(singleDownload->lastError());
            break;
        }

        singleDownload->allowGSMDownload(isGSMDownloadAllowed);
        _downloads.append(singleDownload);
        _downloadsProgress[singleDownload->url()] =
            QPair<qulonglong, qulonglong>(0, singleDownload->totalSize());
        // connect to signals to keep track of the download
        connectToDownloadSignals(singleDownload);
    }
}

void
GroupDownload::cancelAllDownloads() {
    TRACE;
    foreach(FileDownload* download, _downloads) {
        Download::State state = download->state();
        if (state != Download::FINISH && state != Download::ERROR
                && state != Download::CANCEL) {
            download->cancel();
            download->cancelDownload();
        }
    }

    // loop over the finished downloads and remove the files
    foreach(const QString& path, _finishedDownloads) {
        GROUP_LOG(INFO) << "Removing file: " << path;
        _fileManager->remove(path);
    }
}

void
GroupDownload::cancelDownload() {
    TRACE;
    cancelAllDownloads();
    emit canceled(true);
}

void
GroupDownload::pauseDownload() {
    foreach(FileDownload* download, _downloads) {
        Download::State state = download->state();
        if (state == Download::START || state == Download::RESUME) {
            GROUP_LOG(INFO) << "Pausing download of "
                << download->url();
            download->pause();
            download->pauseDownload();
        }
    }
    emit paused(true);
}

void
GroupDownload::resumeDownload() {
    foreach(FileDownload* download, _downloads) {
        Download::State state = download->state();
        if (state == Download::PAUSE) {
            download->resume();
            download->resumeDownload();
        }
    }
    emit resumed(true);
}

void
GroupDownload::startDownload() {
    if (_downloads.count() > 0) {
        foreach(FileDownload* download, _downloads) {
            Download::State state = download->state();
            if (state == Download::IDLE) {
                download->start();
                download->startDownload();
            }
        }
        emit started(true);
    } else {
        emit started(true);
        emit finished(_finishedDownloads);
    }
}

qulonglong
GroupDownload::progress() {
    qulonglong total = 0;
    foreach(FileDownload* download, _downloads) {
        total += download->progress();
    }
    return total;
}

qulonglong
GroupDownload::progress(qulonglong &started, qulonglong &paused,
        qulonglong &finished) {
    // due to an implementation detail on the phone, right now we
    // only have 1 download started if the group was started else 0,
    // all other paused, whatever finished and 0 errors.
    Download::State currentState = state();
    if (currentState == Download::START || currentState == Download::RESUME) {
        started = 1;
    } else {
        started = 0;
    }
    finished = _finishedDownloads.count();
    paused = _downloads.count() - finished - started;
    // no errors
    return 0;
}

qulonglong
GroupDownload::totalSize() {
    qulonglong total = 0;
    foreach(FileDownload* download, _downloads) {
        total += download->totalSize();
    }
    return total;
}

void
GroupDownload::onError(const QString& error) {
    TRACE;
    FileDownload* down = qobject_cast<FileDownload*>(sender());
    // we got an error, cancel all downloads and later remove all the
    // files that we managed to download
    cancelAllDownloads();
    QString errorMsg = down->url().toString() + ":" + error;
    emitError(errorMsg);
}

QString
GroupDownload::getUrlFromSender(QObject* sender) {
    auto down = qobject_cast<FileDownload*>(sender);
    if (down != nullptr) {
        return down->url().toString();
    } else {
        return "";
    }
}

void
GroupDownload::onAuthError(AuthErrorStruct err) {
    auto url = getUrlFromSender(sender());
    if (!url.isEmpty()) {
        emit authError(url, err);
    }
}

void
GroupDownload::onHttpError(HttpErrorStruct err) {
    auto url = getUrlFromSender(sender());
    if (!url.isEmpty()) {
        emit httpError(url, err);
    }
}

void
GroupDownload::onNetworkError(NetworkErrorStruct err) {
    auto url = getUrlFromSender(sender());
    if (!url.isEmpty()) {
        emit networkError(url, err);
    }
}

void
GroupDownload::onProcessError(ProcessErrorStruct err) {
    auto url = getUrlFromSender(sender());
    if (!url.isEmpty()) {
        emit processError(url, err);
    }
}

void
GroupDownload::onProgress(qulonglong received, qulonglong total) {
    FileDownload* down = qobject_cast<FileDownload*>(sender());
    TRACE;
    // TODO(mandel): the result is not real, we need to be smarter make
    // a head request get size and name and the do all this, but atm is
    // 'good enough' get the sender and check if we received
    // progress from it, update its data and recalculate
    QUrl url = down->url();
    if (_downloadsProgress.contains(url)) {
        QPair<qulonglong, qulonglong>& data = _downloadsProgress[url];
        data.first = received;
        if (data.second != total) {
            data.second = total;
        }
    } else {
        _downloadsProgress[url] = QPair<qulonglong, qulonglong>(received,
            total);
    }
    // calculate progress and total using all the downloads, if one of the
    // progress is equal to the total and is not in the finishedDownloads
    // it means that the total is not known and we have to return received
    // == total
    qulonglong totalReceived = 0;
    qulonglong totalTotal = 0;
    QList<QPair<qulonglong, qulonglong> > progressList =
        _downloadsProgress.values();

    for (int index = 0; index < progressList.count(); index++) {
        totalReceived += progressList[index].first;
        totalTotal += progressList[index].second;
    }

    emit Download::progress(totalReceived, totalTotal);
}

void
GroupDownload::onFinished(const QString& file) {
    TRACE << file;
    FileDownload* down = qobject_cast<FileDownload*>(sender());
    _downloadsProgress[down->url()] = QPair<qulonglong, qulonglong>(
        down->totalSize(), down->totalSize());
    _finishedDownloads.append(file);
    GROUP_LOG(INFO) << "Finished downloads "
        << _finishedDownloads;
    // if we have the same number of downloads finished
    // that downloads we are done :)
    if (_downloads.count() == _finishedDownloads.count()) {
        setState(Download::FINISH);
        emit finished(_finishedDownloads);
    }
}

}  // Daemon

}  // DownloadManager

}  // Ubuntu
