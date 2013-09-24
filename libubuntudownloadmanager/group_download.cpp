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
#include "./download_adaptor.h"
#include "./single_download.h"
#include "./uuid_factory.h"
#include "./group_download.h"

/*
 * PRIVATE IMPLEMENTATION
 */

class GroupDownloadPrivate {
    Q_DECLARE_PUBLIC(GroupDownload)

 public:
    GroupDownloadPrivate(QList<GroupDownloadStruct> downloads,
                  QCryptographicHash::Algorithm algo,
                  bool isGSMDownloadAllowed,
                  QSharedPointer<SystemNetworkInfo> networkInfo,
                  QSharedPointer<DownloadFactory> downloadFactory,
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
                  QCryptographicHash::Algorithm algo,
                  bool isGSMDownloadAllowed,
                  QSharedPointer<SystemNetworkInfo> networkInfo,
                  QSharedPointer<DownloadFactory> downFactory,
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
                  QCryptographicHash::Algorithm algo,
                  bool isGSMDownloadAllowed) {
        Q_Q(GroupDownload);
        QVariantMap metadata = q->metadata();
        QMap<QString, QString> headers = q->headers();

        // build downloads and add them to the q, it will take care of
        // starting them etc..
        foreach(GroupDownloadStruct download, downloads) {
            qDebug() << "Creating download for" << download.getUrl();
            QUrl url(download.getUrl());
            QString hash = download.getHash();

            // before we create the download ensure that the url is valid
            // if not we set the error and stop the loop
            if (!url.isValid()) {
                _isValid = false;
                _lastError = QString("Invalid URL: '%1'").arg(url.toString());
                break;
            }

            SingleDownload* singleDownload;
            QVariantMap downloadMetadata = QVariantMap(metadata);
            downloadMetadata[LOCAL_PATH_KEY] = download.getLocalFile();
            qDebug() << "Download metadata is" << downloadMetadata;
            qDebug() << "Group metadata is" << metadata;

            if (hash.isEmpty()) {
                qDebug() << "Creating SingleDownload with no hash.";
                singleDownload = qobject_cast<SingleDownload*>(
                    _downFactory->createDownloadForGroup(q->isConfined(),
                        q->rootPath(), url, downloadMetadata, headers));
            } else {
                qDebug() << "Creating SingleDownload with hash.";
                singleDownload = qobject_cast<SingleDownload*>(
                    _downFactory->createDownloadForGroup(q->isConfined(),
                        q->rootPath(), url, hash, algo, downloadMetadata,
                        headers));
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
            qDebug() << "Signals connected.";
        }
    }

    void cancelDownload(bool emitSignal = true) {
        qDebug() << __PRETTY_FUNCTION__;
        Q_Q(GroupDownload);
        foreach(SingleDownload* download, _downloads) {
            Download::State state = download->state();
            if (state != Download::FINISH && state != Download::ERROR
                    && state != Download::CANCEL) {
                qDebug() << "Canceling download of " << download->url();
                download->cancel();
                download->cancelDownload();
            }
        }

        // loop over the finished downloads and remove the files
        qDebug() << "Finished downloads" << _finishedDownloads;
        foreach(const QString& path, _finishedDownloads) {
            qDebug() << "Removing file" << path;
            _fileManager->remove(path);
        }

        if (emitSignal)
            emit q->canceled(true);
    }

    void pauseDownload() {
        Q_Q(GroupDownload);
        foreach(SingleDownload* download, _downloads) {
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
        foreach(SingleDownload* download, _downloads) {
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
        foreach(SingleDownload* download, _downloads) {
            Download::State state = download->state();
            if (state == Download::IDLE) {
                download->start();
                download->startDownload();
            }
        }
        emit q->started(true);
    }

    bool isValid() {
        return _isValid;
    }

    QString lastError() {
        return _lastError;
    }

    qulonglong progress() {
        qulonglong total = 0;
        foreach(SingleDownload* download, _downloads) {
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
        foreach(SingleDownload* download, _downloads) {
            total += download->totalSize();
        }
        return total;
    }

 private:
    // slots to keep track of the downloads
    void onError(const QString& error) {
        qDebug() << __PRETTY_FUNCTION__;
        Q_Q(GroupDownload);
        SingleDownload* sender = qobject_cast<SingleDownload*>(q->sender());
        // we got an error, cancel all downloads and later remove all the
        // files that we managed to download
        cancelDownload(false);
        QString errorMsg = sender->url().toString() + ":" + error;
        q->emitError(errorMsg);
    }

    void onProgress(qulonglong received, qulonglong total) {
        Q_Q(GroupDownload);
        SingleDownload* sender = qobject_cast<SingleDownload*>(q->sender());
        qDebug() << __PRETTY_FUNCTION__;
        // TODO(mandel): the result is not real, we need to be smarter make
        // a head request get size and name and the do all this, but atm is
        // 'good enough' get the sender and check if we received
        // progress from it, update its data and recalculate
        QUrl url = sender->url();
        qDebug() << "Progress from" << url;
        if (_downloadsProgress.contains(url)) {
            QPair<qulonglong, qulonglong>& data = _downloadsProgress[url];
            data.first = received;
            if (data.second != total) {
                qDebug() << "Updating total!";
                data.second = total;
            }
        } else {
            qDebug() << "First progress signal";
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
        qDebug() << "EMIT group progress" << totalReceived << totalTotal;
        emit down_q->progress(totalReceived, totalTotal);
    }

    void onFinished(const QString& file) {
        Q_Q(GroupDownload);
        qDebug() << __PRETTY_FUNCTION__ << file;
        SingleDownload* sender = qobject_cast<SingleDownload*>(q->sender());
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
    bool _isValid = true;
    QString _lastError;
    QList<SingleDownload*> _downloads;
    QStringList _finishedDownloads;
    QMap<QUrl, QPair<qulonglong, qulonglong> > _downloadsProgress;
    QSharedPointer<SystemNetworkInfo> _networkInfo;
    QSharedPointer<DownloadFactory> _downFactory;
    QSharedPointer<FileManager> _fileManager;
    GroupDownload* q_ptr;
};

/*
 * PUBLIC IMPLEMENTATION
 */

GroupDownload::GroupDownload(const QUuid& id,
                  const QString& path,
                  bool isConfined,
                  const QString& rootPath,
                  QList<GroupDownloadStruct> downloads,
                  QCryptographicHash::Algorithm algo,
                  bool isGSMDownloadAllowed,
                  const QVariantMap& metadata,
                  const QMap<QString, QString>& headers,
                  QSharedPointer<SystemNetworkInfo> networkInfo,
                  QSharedPointer<DownloadFactory> downFactory,
                  QObject* parent)
    : Download(id, path, isConfined, rootPath, metadata, headers,
            networkInfo, parent),
      d_ptr(new GroupDownloadPrivate(downloads, algo, isGSMDownloadAllowed,
            networkInfo, downFactory, this)) {
}

GroupDownload::GroupDownload(const QUuid& id,
                  const QString& path,
                  bool isConfined,
                  const QString& rootPath,
                  QList<GroupDownloadStruct> downloads,
                  QCryptographicHash::Algorithm algo,
                  bool isGSMDownloadAllowed,
                  const QVariantMap& metadata,
                  const QMap<QString, QString>& headers,
                  QSharedPointer<SystemNetworkInfo> networkInfo,
                  QSharedPointer<DownloadFactory> downFactory,
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

bool
GroupDownload::isValid() {
    Q_D(GroupDownload);
    return d->isValid();
}

QString
GroupDownload::lastError() {
    Q_D(GroupDownload);
    return d->lastError();
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

#include "moc_group_download.cpp"
