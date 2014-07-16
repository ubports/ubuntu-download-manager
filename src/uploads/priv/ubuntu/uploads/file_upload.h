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

#ifndef UPLOADER_LIB_FILE_UPLOAD_H
#define UPLOADER_LIB_FILE_UPLOAD_H

#include <QFile>
#include <QNetworkReply>
#include <QProcess>
#include <QSharedPointer>
#include <QUrl>
#include <ubuntu/transfers/errors/auth_error_struct.h>
#include <ubuntu/transfers/errors/http_error_struct.h>
#include <ubuntu/transfers/errors/network_error_struct.h>
#include <ubuntu/transfers/errors/process_error_struct.h>
#include <ubuntu/transfers/system/file_manager.h>
#include <ubuntu/transfers/system/request_factory.h>
#include <ubuntu/transfers/transfer.h>

namespace Ubuntu {

using namespace Transfers;
using namespace Transfers::System;
using namespace Transfers::Errors;

namespace UploadManager {

namespace Daemon {

class FileUpload : public Transfer {
    Q_OBJECT

 public:
    FileUpload(const QString& id,
               const QString& appId,
               const QString& path,
               bool isConfined,
               const QString& rootPath,
               const QUrl& url,
               const QString& filePath,
               const QVariantMap& metadata,
               const QMap<QString, QString>& headers,
               QObject* parent = 0);
    virtual ~FileUpload();

    QObject* adaptor() const;
    void setAdaptor(QObject* adaptor);
    virtual bool pausable() override;
    virtual void cancelTransfer() override;
    virtual void pauseTransfer() override;
    virtual void resumeTransfer() override;
    virtual void startTransfer() override;

 public slots:
    virtual void allowMobileUpload(bool allowed);
    virtual bool isMobileUploadAllowed();
    virtual QVariantMap metadata();
    virtual qulonglong progress();
    virtual void setThrottle(qulonglong speed) override;

 protected:
    virtual QNetworkRequest setRequestHeaders(QNetworkRequest request);
    RequestFactory* _requestFactory;

 private:
    QNetworkRequest buildRequest();
    void connectToReplySignals();
    void disconnectFromReplySignals();
    void emitError(const QString& error);
    void onUploadProgress(qint64 currentProgress, qint64);
    void onError(QNetworkReply::NetworkError);
    QString writeResponseToDisk();
    void onFinished();
    void onSslErrors(const QList<QSslError>&);

 signals:
    void finished(const QString& path);
    void error(const QString& error);
    void authError(AuthErrorStruct error);
    void httpError(HttpErrorStruct error);
    void networkError(NetworkErrorStruct error);
    void processError(ProcessErrorStruct error);
    void progress(qulonglong uploded, qulonglong total);

 private:
    qulonglong _progress = 0;
    QUrl _url;
    QString _filePath;
    QVariantMap _metadata;
    QMap<QString, QString> _headers;
    QObject* _adaptor = nullptr;
    NetworkReply* _reply = nullptr;
    File* _currentData = nullptr;
};

}  // Daemon

}  // UploadManager

}  // Ubuntu


#endif
