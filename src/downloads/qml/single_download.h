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

#ifndef FILE_DOWNLOAD_H
#define FILE_DOWNLOAD_H

#include <QObject>
#include <ubuntu/download_manager/download.h>
#include <ubuntu/download_manager/error.h>
#include <ubuntu/download_manager/manager.h>
#include "metadata.h"
#include "download_error.h"

namespace Ubuntu {

namespace DownloadManager {

class SingleDownload : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoStart READ autoStart WRITE setAutoStart)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorChanged)
    Q_PROPERTY(bool isCompleted READ isCompleted NOTIFY isCompletedChanged)
    Q_PROPERTY(bool downloadInProgress READ downloadInProgress NOTIFY downloadInProgressChanged)
    Q_PROPERTY(bool allowMobileDownload READ allowMobileDownload WRITE setAllowMobileDownload NOTIFY allowMobileDownloadChanged)
    Q_PROPERTY(qulonglong throttle READ throttle WRITE setThrottle NOTIFY throttleChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool downloading READ downloading NOTIFY downloadingChanged)
    Q_PROPERTY(QString downloadId READ downloadId NOTIFY downloadIdChanged)
    Q_PROPERTY(QVariantMap headers READ headers WRITE setHeaders NOTIFY headersChanged)
    Q_PROPERTY(Ubuntu::DownloadManager::Metadata* metadata READ metadata WRITE setMetadata NOTIFY metadataChanged)

 public:
    explicit SingleDownload(QObject *parent = 0);

    Q_INVOKABLE void start();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void resume();
    Q_INVOKABLE void cancel();
    Q_INVOKABLE void download(QString url);

    void startDownload();

    // getters
    bool isCompleted() const {
        return m_completed;
    }

    DownloadError& error() {
        return m_error;
    }

    QString errorMessage() const {
        return m_error.message();
    }

    qulonglong throttle() const;

    bool allowMobileDownload() const;

    int progress() const {
        return m_progress;
    }

    bool downloading() const {
        return m_downloading;
    }

    bool downloadInProgress() const {
        return m_downloadInProgress;
    }

    bool autoStart() const {
        return m_autoStart;
    }

    QString downloadId() const;
    QVariantMap headers() const;
    Metadata* metadata() const;

    // setters
    void setAllowMobileDownload(bool value);
    void setThrottle(qulonglong value);
    void setHeaders(QVariantMap headers);
    void setMetadata(Metadata* metadata);

    // only property that does not access the download obj
    void setAutoStart(bool value) {
        m_autoStart = value;
    }

 protected:
    SingleDownload(Download* down, Manager* man, QObject *parent = 0);

 signals:
    void isCompletedChanged();
    void allowMobileDownloadChanged();
    void throttleChanged();
    void progressChanged();
    void downloadingChanged();
    void downloadInProgressChanged();
    void downloadIdChanged();
    void headersChanged();
    void metadataChanged();

    void canceled(bool success);
    void finished(const QString& path);
    void paused(bool success);
    void processing(const QString &path);
    void progressReceived(qulonglong received, qulonglong total);
    void resumed(bool success);
    void started(bool success);
    void errorFound(DownloadError& error);
    void errorChanged();

public slots:
    void registerError(Error* error);
    void bindDownload(Download* download);
    void unbindDownload(Download* download);
    void onFinished(const QString& path);
    void onProgress(qulonglong received, qulonglong total);
    void onPaused(bool wasPaused);
    void onResumed(bool wasResumed);
    void onStarted(bool wasStarted);
    void onCanceled(bool wasCanceled);

private:
    QString getErrorType(Error::Type type) {
        switch (type) {
            case Error::Auth:
                return QString("Auth");
            case Error::DBus:
                return QString("DBus");
            case Error::Http:
                return QString("Http");
            case Error::Network:
                return QString("Network");
            case Error::Process:
                return QString("Process");
            default:
                return QString();
        }
    }

private:
    bool m_autoStart = false;
    bool m_completed = false;
    bool m_downloading = false;
    bool m_dirty = false;
    bool m_downloadInProgress = false;
    int m_progress;
    bool m_mobile = true;
    qulonglong m_throttle = 0;
    QVariantMap m_headers;
    Metadata* m_metadata = nullptr;
    DownloadError m_error;
    Download* m_download = nullptr;
    Manager* m_manager = nullptr;

};

}
}

#endif // FILE_DOWNLOAD_H
