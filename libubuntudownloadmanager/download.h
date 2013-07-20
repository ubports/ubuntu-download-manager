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

#ifndef DOWNLOADER_LIB_APP_DOWNLOAD_H
#define DOWNLOADER_LIB_APP_DOWNLOAD_H

#include <QObject>
#include <QByteArray>
#include <QBuffer>
#include <QCryptographicHash>
#include <QNetworkReply>
#include <QUrl>
#include <QUuid>
#include "system_network_info.h"
#include "request_factory.h"
#include "app-downloader-lib_global.h"

class DownloadPrivate;
class APPDOWNLOADERLIBSHARED_EXPORT Download : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Download)
public:

    enum State
    {
        IDLE,
        STARTED,
        PAUSED,
        RESUMED,
        CANCELED,
        FINISHED
    };

    explicit Download(const QUuid& id, const QString& path, const QUrl& url, const QVariantMap& metadata,
        const QMap<QString, QString>& headers, SystemNetworkInfo* networkInfo, RequestFactory* nam, QObject* parent=0);
    explicit Download(const QUuid& id, const QString& path, const QUrl& url, const QString& hash, QCryptographicHash::Algorithm algo,
        const QVariantMap& metadata, const QMap<QString, QString>& headers, SystemNetworkInfo* networkInfo, RequestFactory* nam,
        QObject* parent=0);

    // gets for internal state
    QUuid downloadId();
    QString path();
    QUrl url();
    Download::State state();
    void setState(Download::State state);
    QString filePath();
    QString hash();
    QCryptographicHash::Algorithm hashAlgorithm();
    QMap<QString, QString> headers();
    virtual bool canDownload();

    // methods that do perform the download
    virtual void cancelDownload();
    virtual void pauseDownload();
    virtual void resumeDownload();
    virtual void startDownload();
    static Download* fromMetadata(const QString &path, RequestFactory* nam);


public slots:
    // slots that are exposed via dbus, they just change the state, the downloader
    // takes care of the actual download operations
    QVariantMap metadata();
    qlonglong progress();
    qlonglong totalSize();
    virtual void setThrottle(qlonglong speed);
    virtual qlonglong throttle();
    void allowGSMDownload(bool allowed);
    bool isGSMDownloadAllowed();
    void cancel();
    void pause();
    void resume();
    void start();

signals:
    // signals that are exposed via dbus
    void canceled(bool success);
    void error(const QString& error);
    void finished(const QString& path);
    void paused(bool success);
    void progress(qlonglong received, qlonglong total);
    void resumed(bool success);
    void started(bool success);

    // internal signals used for the download queue
    void stateChanged();

private:
    explicit Download();
    // private slots used to keep track of the qnetwork reply state

    Q_PRIVATE_SLOT(d_func(), void onDownloadProgress(qint64, qint64))
    Q_PRIVATE_SLOT(d_func(), void onError(QNetworkReply::NetworkError))
    Q_PRIVATE_SLOT(d_func(), void onFinished())
    Q_PRIVATE_SLOT(d_func(), void onSslErrors(const QList<QSslError>&))

private:
    // use pimpl so that we can mantains ABI compatibility
    DownloadPrivate* d_ptr;
};

#endif // APP_DOWNLOAD_H
