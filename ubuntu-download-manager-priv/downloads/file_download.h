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

#ifndef DOWNLOADER_LIB_SINGLE_DOWNLOAD_H
#define DOWNLOADER_LIB_SINGLE_DOWNLOAD_H

#include <QFile>
#include <QNetworkReply>
#include <QProcess>
#include <QSharedPointer>
#include <QUrl>
#include <ubuntu/download_manager/http_error_struct.h>
#include <ubuntu/download_manager/network_error_struct.h>
#include <ubuntu/download_manager/process_error_struct.h>
#include "app-downloader-lib_global.h"
#include "downloads/download.h"
#include "system/file_manager.h"

#define LOCAL_PATH_KEY "local-path"

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

class FileDownload : public Download {
    Q_OBJECT

 public:
    FileDownload(const QString& id,
                 const QString& path,
                 bool isConfined,
                 const QString& rootPath,
                 const QUrl& url,
                 const QVariantMap& metadata,
                 const QMap<QString, QString>& headers,
                 QObject* parent = 0);
    FileDownload(const QString& id,
                 const QString& path,
                 bool isConfined,
                 const QString& rootPath,
                 const QUrl& url,
                 const QString& hash,
                 const QString& algo,
                 const QVariantMap& metadata,
                 const QMap<QString, QString>& headers,
                 QObject* parent = 0);
    virtual ~FileDownload();

    // gets for internal state
    QUrl url() const {
        return _url;
    }

    QString filePath() const {
        return _filePath;
    }

    QString hash() const {
        return _hash;
    }

    QCryptographicHash::Algorithm hashAlgorithm() const {
        return _algo;
    }

    // methods that do perform the download
    virtual void cancelDownload() override;
    virtual void pauseDownload() override;
    virtual void resumeDownload() override;
    virtual void startDownload() override;

 public slots:  // NOLINT(whitespace/indent)
    qulonglong progress() override;
    qulonglong totalSize() override;
    virtual void setThrottle(qulonglong speed) override;

 signals:
    void finished(const QString& path);
    void httpError(HttpErrorStruct error);
    void networkError(NetworkErrorStruct error);
    void processError(ProcessErrorStruct error);

 protected:
    void emitError(const QString& error) override;

 private:
    QNetworkRequest buildRequest();
    void cleanUpCurrentData();
    void connectToReplySignals();
    void disconnectFromReplySignals();
    bool flushFile();
    void init();
    void onDownloadProgress(qint64 currentProgress, qint64);
    void onError(QNetworkReply::NetworkError);
    void onFinished();
    void onSslErrors(const QList<QSslError>&);
    void onProcessError(QProcess::ProcessError error);
    void onProcessFinished(int exitCode,
                           QProcess::ExitStatus exitStatus);
    void onOnlineStateChanged(bool);
    QString getSaveFileName();
    QString uniqueFilePath(QString path);

 private:
    bool _downloading = false;
    bool _connected = false;
    qulonglong _totalSize = 0;
    QUrl _url;
    QString _filePath;
    QString _hash;
    QCryptographicHash::Algorithm _algo;
    NetworkReply* _reply = NULL;
    File* _currentData = NULL;
    RequestFactory* _requestFactory;
};

}  // Daemon

}  // DownloadManager

}  // Ubuntu

#endif  // DOWNLOADER_LIB_SINGLE_DOWNLOAD_H
