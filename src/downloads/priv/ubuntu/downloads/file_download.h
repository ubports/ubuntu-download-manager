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

#include <QDBusContext>
#include <QFile>
#include <QNetworkReply>
#include <QProcess>
#include <QSharedPointer>
#include <QUrl>
#include <ubuntu/transfers/metadata.h>
#include <ubuntu/transfers/errors/auth_error_struct.h>
#include <ubuntu/transfers/errors/http_error_struct.h>
#include <ubuntu/transfers/errors/network_error_struct.h>
#include <ubuntu/transfers/errors/process_error_struct.h>
#include <ubuntu/transfers/system/file_manager.h>
#include <ubuntu/transfers/system/filename_mutex.h>
#include "download.h"

namespace Ubuntu {

using namespace Transfers::Errors;

namespace DownloadManager {

namespace Daemon {

class FileDownload : public Download, public QDBusContext {
    Q_OBJECT

 public:
    FileDownload(const QString& id,
                 const QString& appId,
                 const QString& path,
                 bool isConfined,
                 const QString& rootPath,
                 const QUrl& url,
                 const QVariantMap& metadata,
                 const QMap<QString, QString>& headers,
                 QObject* parent = 0);
    FileDownload(const QString& id,
                 const QString& appId,
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
    virtual QUrl url() const {
        return _url;
    }

    virtual QString filePath() const {
        return _filePath;
    }

    virtual QString hash() const {
        return _hash;
    }

    virtual QCryptographicHash::Algorithm hashAlgorithm() const {
        return _algo;
    }

    // methods that do perform the download
    virtual void cancelTransfer() override;
    virtual void pauseTransfer() override;
    virtual void resumeTransfer() override;
    virtual void startTransfer() override;

 public slots:  // NOLINT(whitespace/indent)
    qulonglong progress() override;
    qulonglong totalSize() override;
    virtual void setThrottle(qulonglong speed) override;
    virtual void setDestinationDir(const QString& path);
    virtual void setHeaders(StringMap headers) override;
    virtual void setMetadata(const QVariantMap& metadata) override;

 signals:
    void finished(const QString& path);
    void authError(AuthErrorStruct error);
    void httpError(HttpErrorStruct error);
    void networkError(NetworkErrorStruct error);
    void processError(ProcessErrorStruct error);
    void propertiesChanged(const QVariantMap& changes);

 protected:
    void emitError(const QString& error) override;

 protected:
    RequestFactory* _requestFactory;

 private:
    // helper methods
    QNetworkRequest buildRequest();
    void cleanUpCurrentData();
    void connectToReplySignals();
    void disconnectFromReplySignals();
    void emitFinished();
    bool flushFile();
    bool hashIsValid();
    void init();
    void initFileNames();
    void unlockFilePath();
    void updateFileNamePerContentDisposition();

    // slots used to react to signals
    void onDownloadProgress(qint64 currentProgress, qint64);
    void onError(QNetworkReply::NetworkError);
    void onRedirect(QUrl redirect);
    void onDownloadCompleted();
    void onFinished();
    void onSslErrors(const QList<QSslError>&);
    void onProcessError(QProcess::ProcessError error);
    void onProcessFinished(int exitCode,
                           QProcess::ExitStatus exitStatus);
    void onOnlineStateChanged(bool);
    void onPropertiesChanged(const QVariantMap& changes);


 private:
    bool _downloading = false;
    bool _connected = false;
    qulonglong _totalSize = 0;
    QUrl _url;
    QString _basename;
    QString _filePath;
    QString _tempFilePath;
    QString _hash;
    QCryptographicHash::Algorithm _algo;
    NetworkReply* _reply = nullptr;
    File* _currentData = nullptr;
    FileNameMutex* _fileNameMutex = nullptr;
    QList<QUrl> _visitedUrls;
};

}  // Daemon

}  // DownloadManager

}  // Ubuntu

#endif  // DOWNLOADER_LIB_SINGLE_DOWNLOAD_H
