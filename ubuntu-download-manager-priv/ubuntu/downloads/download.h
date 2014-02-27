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

#ifndef DOWNLOADER_LIB_DOWNLOAD_H
#define DOWNLOADER_LIB_DOWNLOAD_H

#include <QNetworkAccessManager>
#include <QObject>
#include <QProcess>
#include <QSharedPointer>
#include "ubuntu/transfers/system/process_factory.h"
#include "ubuntu/transfers/system/request_factory.h"
#include "ubuntu/transfers/system/system_network_info.h"


namespace Ubuntu {

using namespace Transfers::System;

namespace DownloadManager {

namespace Daemon {

class Download : public QObject {
    Q_OBJECT

 public:
    enum State {
        IDLE,
        START,
        PAUSE,
        RESUME,
        CANCEL,
        FINISH,
        ERROR
    };

    Download(const QString& id,
             const QString& path,
             bool isConfined,
             const QString& rootPath,
             const QVariantMap& metadata,
             const QMap<QString, QString>& headers,
             QObject* parent = 0);

    virtual ~Download();

    QString downloadId() const {
        return _id;
    }

    QString path() const {
        return _dbusPath;
    }

    bool isConfined() const {
        return _isConfined;
    }

    QString rootPath() const {
        return _rootPath;
    }

    virtual Download::State state() const {
        return _state;
    }

    void setState(Download::State state);

    QObject* adaptor() const {
        return _adaptor;
    }
    void setAdaptor(QObject* adaptor);

    QMap<QString, QString> headers() const {
        return _headers;
    }

    virtual bool canDownload();

    virtual bool isValid() const {
        return _isValid;
    }

    bool addToQueue() const {
        return _addToQueue;
    }

    virtual QString lastError() const {
        return _lastError;
    }

    // methods to be overriden by the children
    virtual void cancelDownload() = 0;
    virtual void pauseDownload() = 0;
    virtual void resumeDownload() = 0;
    virtual void startDownload() = 0;

 public slots:  // NOLINT(whitespace/indent)
    // slots that are exposed via dbus, they just change the state,
    // the downloader takes care of the actual download operations
    virtual QVariantMap metadata() const {
        return _metadata;
    }

    virtual void setThrottle(qulonglong speed);

    virtual qulonglong throttle() {
        return _throttle;
    }

    virtual void allowGSMDownload(bool allowed);
    virtual bool isGSMDownloadAllowed();

    virtual inline void cancel() {
        setState(Download::CANCEL);
        if(!_addToQueue)
            cancelDownload();
    }

    virtual inline void pause() {
        setState(Download::PAUSE);
        if(!_addToQueue)
            pauseDownload();
    }

    virtual inline void resume() {
        setState(Download::RESUME);
        if(!_addToQueue)
            resumeDownload();
    }

    virtual inline void start() {
        setState(Download::START);
        if(!_addToQueue)
            startDownload();
    }

    // slots to be implemented by the children
    virtual qulonglong progress() = 0;
    virtual qulonglong totalSize() = 0;

 signals:
    // signals that are exposed via dbus
    void canceled(bool success);
    void error(const QString& error);
    void paused(bool success);
    void processing(const QString& file);
    void resumed(bool success);
    void started(bool success);
    void progress(qulonglong received, qulonglong total);

    // internal signals
    void stateChanged();
    void throttleChanged();

 protected:
    void setIsValid(bool isValid);
    void setAddToQueue(bool addToQueue);
    void setLastError(const QString& lastError);
    virtual void emitError(const QString& error);

 private:
    bool _isValid = true;
    bool _addToQueue = true;
    QString _lastError = "";
    QString _id;
    qulonglong _throttle;
    bool _allowGSMDownload;
    Download::State _state;
    QString _dbusPath;
    bool _isConfined;
    QString _rootPath;
    QVariantMap _metadata;
    QMap<QString, QString> _headers;
    SystemNetworkInfo* _networkInfo;
    QObject* _adaptor = nullptr;
};

}  // Daemon

}  // DownloadManager

}  // Ubuntu

#endif  // DOWNLOADER_LIB_APP_DOWNLOAD_H
