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

#ifndef DOWNLOADER_LIB_SINGLE_DOWNLOAD_H
#define DOWNLOADER_LIB_SINGLE_DOWNLOAD_H

#include <QNetworkReply>
#include <QProcess>
#include <QSharedPointer>
#include <QUrl>
#include <QUuid>
#include "./app-downloader-lib_global.h"
#include "./download.h"

#define LOCAL_PATH_KEY "local-path"

class SingleDownloadPrivate;
class APPDOWNLOADERLIBSHARED_EXPORT SingleDownload : public Download {
    Q_OBJECT
    Q_DECLARE_PRIVATE(SingleDownload)

 public:
    SingleDownload(const QUuid& id,
                   const QString& path,
                   bool isConfined,
                   const QString& rootPath,
                   const QUrl& url,
                   const QVariantMap& metadata,
                   const QMap<QString, QString>& headers,
                   QSharedPointer<SystemNetworkInfo> networkInfo,
                   QSharedPointer<RequestFactory> nam,
                   QSharedPointer<ProcessFactory> processFactory,
                   QObject* parent = 0);
    SingleDownload(const QUuid& id,
                   const QString& path,
                   bool isConfined,
                   const QString& rootPath,
                   const QUrl& url,
                   const QString& hash,
                   const QString& algo,
                   const QVariantMap& metadata,
                   const QMap<QString, QString>& headers,
                   QSharedPointer<SystemNetworkInfo> networkInfo,
                   QSharedPointer<RequestFactory> nam,
                   QSharedPointer<ProcessFactory> processFactory,
                   QObject* parent = 0);

    // gets for internal state
    QUrl url();
    QString filePath();
    QString hash();
    QCryptographicHash::Algorithm hashAlgorithm();

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

 private:
    // private slots used to keep track of the qnetwork reply state

    Q_PRIVATE_SLOT(d_func(), void onDownloadProgress(qint64, qint64))
    Q_PRIVATE_SLOT(d_func(), void onError(QNetworkReply::NetworkError))
    Q_PRIVATE_SLOT(d_func(), void onFinished())
    Q_PRIVATE_SLOT(d_func(), void onSslErrors(const QList<QSslError>&))

    // private slots used to keep track of the post download command

    Q_PRIVATE_SLOT(d_func(), void onProcessError(QProcess::ProcessError error))
    Q_PRIVATE_SLOT(d_func(), void onProcessFinished(int exitCode,
                                              QProcess::ExitStatus exitStatus))

 private:
    // use pimpl so that we can mantains ABI compatibility
    SingleDownloadPrivate* d_ptr;
};

#endif  // DOWNLOADER_LIB_SINGLE_DOWNLOAD_H
