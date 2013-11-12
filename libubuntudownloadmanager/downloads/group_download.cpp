/*
 * copyright 2013 2013 canonical ltd.
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

#include <QDebug>
#include "downloads/download_adaptor.h"
#include "downloads/file_download.h"
#include "downloads/group_download.h"
#include "system/hash_algorithm.h"
#include "system/logger.h"
#include "system/uuid_factory.h"

namespace Ubuntu {

namespace DownloadManager {

/*
 * PRIVATE IMPLEMENTATION
 */

class GroupDownloadPrivate {
    Q_DECLARE_PUBLIC(GroupDownload)

 public:
    GroupDownloadPrivate(QList<GroupDownloadStruct> downloads,
                  const QString& algo,
                  bool isGSMDownloadAllowed,
                  QSharedPointer<SystemNetworkInfo> networkInfo,
                  QSharedPointer<Factory> downloadFactory,
                  GroupDownload* parent)
        : _downloads(),
          _finishedDownloads(),
          _downloadsProgress(),
          _networkInfo(networkInfo),
          _downFactory(downloadFactory),
          q_ptr(parent) {
        _fileManager = QSharedPointer<FileManager>(new FileManager());
        init(downloads, algo, isGSMDownloadAllowed);
    }

    GroupDownloadPrivate(QList<GroupDownloadStruct> downloads,
                  const QString& algo,
                  bool isGSMDownloadAllowed,
                  QSharedPointer<SystemNetworkInfo> networkInfo,
                  QSharedPointer<Factory> downFactory,
                  QSharedPointer<FileManager> fileManager,
                  GroupDownload* parent)
        : _downloads(),
          _finishedDownloads(),
          _downloadsProgress(),
          _networkInfo(networkInfo),
          _downFactory(downFactory),
          _fileManager(fileManager),
          q_ptr(parent) {
        init(downloads, algo, isGSMDownloadAllowed);
    }

    void init(QList<GroupDownloadStruct> downloads,
                  const QString& algo,
                  bool isGSMDownloadAllowed) {
        Q_Q(GroupDownload);
        QVariantMap metadata = q->metadata();
        QMap<QString, QString> headers = q->headers();
        QStringList localPaths;

        // build downloads and add them to the q, it will take care of
        // starting them etc..
        foreach(GroupDownloadStruct download, downloads) {
            QUrl url(download.getUrl());
            QString hash = download.getHash();

            FileDownload* singleDownload;
            QVariantMap downloadMetadata = QVariantMap(metadata);
            downloadMetadata[LOCAL_PATH_KEY] = download.getLocalFile();

            if (hash.isEmpty()) {
                singleDownload = qobject_cast<FileDownload*>(
                    _downFactory->createDownloadForGroup(q->isConfined(),
                        q->rootPath(), url, downloadMetadata, headers));
            } else {

                if (!HashAlgorithm::isValidAlgo(algo)) {
                    q->setIsValid(false);
                    q->setLastError(QString("Invalid hash algorithm: '%1'").arg(algo));
                }

                singleDownload = qobject_cast<FileDownload*>(
                    _downFactory->createDownloadForGroup(q->isConfined(),
                        q->rootPath(), url, hash, algo, downloadMetadata,
                        headers));
            }

            // ensure that the local path is not used by any other download
	    // in this group.
            QString localFilePath = singleDownload->filePath();
            if (localPaths.contains(localFilePath)) {
                q->setIsValid(false);
                q->setLastError("Duplicated local path passed: " + localFilePath);
                break;
            } else {
                localPaths.append(localFilePath);
            }

            // check that the download is valid, if it is not set to invalid
            // and use the last error from the download
            if (!singleDownload->isValid()) {
                q->setIsValid(false);
                q->setLastError(singleDownload->lastError());
                break;
            }

            singleDownload->allowGSMDownload(isGSMDownloadAllowed);
            _downloads.append(singleDownload);
            _downloadsProgress[singleDownload->url()] =
                QPair<qulonglong, qulonglong>(0, singleDownload->totalSize());
            // connect to signals to keep track of the download
            q->connect(singleDownload, SIGNAL(error(const QString&)),
                q, SLOT(onError(const QString&)));
            q->connect(singleDownload, SIGNAL(progress(qulonglong, qulonglong)),
                q, SLOT(onProgress(qulonglong, qulonglong)));
            q->connect(singleDownload, SIGNAL(finished(const QString&)),
                q, SLOT(onFinished(const QString&)));
            q->connect(singleDownload, SIGNAL(processing(QString)),
                q, SIGNAL(processing(QString)));
        }
    }

    void cancelDownload(bool emitSignal = true) {
        TRACE;
        Q_Q(GroupDownload);
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
            qDebug() << "Removing file" << path;
            _fileManager->remove(path);
        }

        if (emitSignal)
            emit q->canceled(true);
    }

    void pauseDownload() {
        Q_Q(GroupDownload);
        foreach(FileDownload* download, _downloads) {
            Download::State state = download->state();
            if (state == Download::START || state == Download::RESUME) {
                qDebug() << "Pausing download of " << download->url();
                download->pause();
                download->pauseDownload();
            }
        }
        emit q->paused(true);
    }

    void resumeDownload() {
        Q_Q(GroupDownload);
        foreach(FileDownload* download, _downloads) {
            Download::State state = download->state();
            if (state == Download::PAUSE) {
                download->resume();
                download->resumeDownload();
            }
        }
        emit q->resumed(true);
    }

    void startDownload() {
        Q_Q(GroupDownload);
        if (_downloads.count() > 0) {
            foreach(FileDownload* download, _downloads) {
                Download::State state = download->state();
                if (state == Download::IDLE) {
                    download->start();
                    download->startDownload();
                }
            }
            emit q->started(true);
        } else {
            emit q->started(true);
            emit q->finished(_finishedDownloads);
        }
    }

    qulonglong progress() {
        qulonglong total = 0;
        foreach(FileDownload* download, _downloads) {
            total += download->progress();
        }
        return total;
    }

    qulonglong progress(qulonglong &started, qulonglong &paused,
            qulonglong &finished) {
        Q_Q(GroupDownload);
        // due to an implementation detail on the phone, right now we
        // only have 1 download started if the group was started else 0,
        // all other paused, whatever finished and 0 errors.
        Download::State state = q->state();
        if (state == Download::START || state == Download::RESUME) {
            started = 1;
        } else {
            started = 0;
        }
        finished = _finishedDownloads.count();
        paused = _downloads.count() - finished - started;
        // no errors
        return 0;
    }

    qulonglong totalSize() {
        qulonglong total = 0;
        foreach(FileDownload* download, _downloads) {
            total += download->totalSize();
        }
        return total;
    }

 private:
    // slots to keep track of the downloads
    void onError(const QString& error) {
        TRACE;
        Q_Q(GroupDownload);
        FileDownload* sender = qobject_cast<FileDownload*>(q->sender());
        // we got an error, cancel all downloads and later remove all the
        // files that we managed to download
        cancelDownload(false);
        QString errorMsg = sender->url().toString() + ":" + error;
        q->emitError(errorMsg);
    }

    void onProgress(qulonglong received, qulonglong total) {
        Q_Q(GroupDownload);
        FileDownload* sender = qobject_cast<FileDownload*>(q->sender());
        TRACE;
        // TODO(mandel): the result is not real, we need to be smarter make
        // a head request get size and name and the do all this, but atm is
        // 'good enough' get the sender and check if we received
        // progress from it, update its data and recalculate
        QUrl url = sender->url();
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

        Download* down_q = reinterpret_cast<Download*>(q);
        emit down_q->progress(totalReceived, totalTotal);
    }

    void onFinished(const QString& file) {
        Q_Q(GroupDownload);
        TRACE << file;
        FileDownload* sender = qobject_cast<FileDownload*>(q->sender());
        _downloadsProgress[sender->url()] = QPair<qulonglong, qulonglong>(
            sender->totalSize(), sender->totalSize());
        _finishedDownloads.append(file);
        qDebug() << "Finished downloads" << _finishedDownloads;
        // if we have the same number of downloads finished
        // that downloads we are done :)
        if (_downloads.count() == _finishedDownloads.count()) {
            q->setState(Download::FINISH);
            emit q->finished(_finishedDownloads);
        }
    }

 private:
    QList<FileDownload*> _downloads;
    QStringList _finishedDownloads;
    QMap<QUrl, QPair<qulonglong, qulonglong> > _downloadsProgress;
    QSharedPointer<SystemNetworkInfo> _networkInfo;
    QSharedPointer<Factory> _downFactory;
    QSharedPointer<FileManager> _fileManager;
    GroupDownload* q_ptr;
};

/*
 * PUBLIC IMPLEMENTATION
 */

GroupDownload::GroupDownload(const QString& id,
                  const QString& path,
                  bool isConfined,
                  const QString& rootPath,
                  QList<GroupDownloadStruct> downloads,
                  const QString& algo,
                  bool isGSMDownloadAllowed,
                  const QVariantMap& metadata,
                  const QMap<QString, QString>& headers,
                  QSharedPointer<SystemNetworkInfo> networkInfo,
                  QSharedPointer<Factory> downFactory,
                  QObject* parent)
    : Download(id, path, isConfined, rootPath, metadata, headers,
            networkInfo, parent),
      d_ptr(new GroupDownloadPrivate(downloads, algo, isGSMDownloadAllowed,
            networkInfo, downFactory, this)) {
}

GroupDownload::GroupDownload(const QString& id,
                  const QString& path,
                  bool isConfined,
                  const QString& rootPath,
                  QList<GroupDownloadStruct> downloads,
                  const QString& algo,
                  bool isGSMDownloadAllowed,
                  const QVariantMap& metadata,
                  const QMap<QString, QString>& headers,
                  QSharedPointer<SystemNetworkInfo> networkInfo,
                  QSharedPointer<Factory> downFactory,
                  QSharedPointer<FileManager> fileManager,
                  QObject* parent)
    : Download(id, path, isConfined, rootPath, metadata, headers,
            networkInfo, parent),
      d_ptr(new GroupDownloadPrivate(downloads, algo, isGSMDownloadAllowed,
            networkInfo, downFactory, fileManager, this)) {
}

void
GroupDownload::cancelDownload() {
    Q_D(GroupDownload);
    d->cancelDownload();
}

void
GroupDownload::pauseDownload() {
    Q_D(GroupDownload);
    d->pauseDownload();
}

void
GroupDownload::resumeDownload() {
    Q_D(GroupDownload);
    d->resumeDownload();
}

void
GroupDownload::startDownload() {
    Q_D(GroupDownload);
    d->startDownload();
}

qulonglong
GroupDownload::progress() {
    Q_D(GroupDownload);
    return d->progress();
}

qulonglong
GroupDownload::progress(qulonglong &started,
                        qulonglong &paused,
                        qulonglong &finished) {
    Q_D(GroupDownload);
    return d->progress(started, paused, finished);
}

qulonglong
GroupDownload::totalSize() {
    Q_D(GroupDownload);
    return d->totalSize();
}

}  // DownloadManager

}  // Ubuntu

#include "moc_group_download.cpp"
