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

#include <QDebug>
#include <QStringList>
#include "./download.h"

/**
 * PRIVATE IMPLEMENATION
 */

class DownloadPrivate {
    Q_DECLARE_PUBLIC(Download)

 public:
    DownloadPrivate(const QUuid& id,
                    const QString& path,
                    const QVariantMap& metadata,
                    const QMap<QString, QString>& headers,
                    SystemNetworkInfo* networkInfo,
                    Download* parent)
        : _id(id),
          _throttle(0),
          _allowGSMDownload(true),
          _state(Download::IDLE),
          _dbusPath(path),
          _metadata(metadata),
          _headers(headers),
          _networkInfo(networkInfo),
          q_ptr(parent) {
    }

    ~DownloadPrivate() {}

    QUuid downloadId() const {
        return _id;
    }

    QString path() const {
        return _dbusPath;
    }

    Download::State state() {
        return _state;
    }

    void setState(Download::State state) {
        Q_Q(Download);
        _state = state;
        emit q->stateChanged();
    }

    QMap<QString, QString> headers() const {
        return _headers;
    }

    bool canDownload() {
        qDebug() << __PRETTY_FUNCTION__;
        QNetworkInfo::NetworkMode mode = _networkInfo->currentNetworkMode();
        switch (mode) {
            case QNetworkInfo::UnknownMode:
                qWarning() << "Network Mode unknown!";
                return _allowGSMDownload;
                break;
            case QNetworkInfo::GsmMode:
            case QNetworkInfo::CdmaMode:
            case QNetworkInfo::WcdmaMode:
            case QNetworkInfo::WimaxMode:
            case QNetworkInfo::TdscdmaMode:
            case QNetworkInfo::LteMode:
                return _allowGSMDownload;
            case QNetworkInfo::WlanMode:
            case QNetworkInfo::EthernetMode:
            case QNetworkInfo::BluetoothMode:
                return true;
            default:
                return false;
        }
    }

    QVariantMap metadata() {
        return _metadata;
    }

    void setThrottle(qulonglong speed) {
        _throttle = speed;
    }

    qulonglong throttle() {
        return _throttle;
    }

    void allowGSMDownload(bool allowed) {
        Q_Q(Download);
        if (_allowGSMDownload != allowed) {
            _allowGSMDownload = allowed;
            // emit the signals so that they q knows what to do
            emit q->stateChanged();
        }
    }

    bool isGSMDownloadAllowed() {
        return _allowGSMDownload;
    }

    void cancel() {
        setState(Download::CANCEL);
    }

    void pause() {
        setState(Download::PAUSE);
    }

    void resume() {
        setState(Download::RESUME);
    }

    void start() {
        setState(Download::START);
    }

    void emitError(const QString& error) {
        qDebug() << __PRETTY_FUNCTION__ << error;
        Q_Q(Download);
        setState(Download::ERROR);
        emit q->error(error);
    }

 private:
    QUuid _id;
    qulonglong _throttle;
    bool _allowGSMDownload;
    Download::State _state;
    QString _dbusPath;
    QVariantMap _metadata;
    QMap<QString, QString> _headers;
    SystemNetworkInfo* _networkInfo;
    Download* q_ptr;
};

/**
 * PUBLIC IMPLEMENTATION
 */

Download::Download(const QUuid& id,
                   const QString& path,
                   const QVariantMap& metadata,
                   const QMap<QString, QString>& headers,
                   SystemNetworkInfo* networkInfo,
                   QObject* parent)
    : QObject(parent),
      d_ptr(new DownloadPrivate(id, path, metadata, headers,
                  networkInfo, this)) {
}

QUuid
Download::downloadId() {
    Q_D(Download);
    return d->downloadId();
}

QString
Download::path() {
    Q_D(Download);
    return d->path();
}

Download::State
Download::state() {
    Q_D(Download);
    return d->state();
}

void
Download::setState(Download::State state) {
    Q_D(Download);
    d->setState(state);
}

QMap<QString, QString>
Download::headers() {
    Q_D(Download);
    return d->headers();
}

bool
Download::canDownload() {
    Q_D(Download);
    return d->canDownload();
}

QVariantMap
Download::metadata() {
    Q_D(Download);
    return d->metadata();
}

void
Download::setThrottle(qulonglong speed) {
    Q_D(Download);
    return d->setThrottle(speed);
}

qulonglong
Download::throttle() {
    Q_D(Download);
    return d->throttle();
}

void
Download::allowGSMDownload(bool allowed) {
    Q_D(Download);
    d->allowGSMDownload(allowed);
}

bool
Download::isGSMDownloadAllowed() {
    Q_D(Download);
    return d->isGSMDownloadAllowed();
}

void
Download::cancel() {
    Q_D(Download);
    d->cancel();
}

void
Download::pause() {
    Q_D(Download);
    d->pause();
}

void
Download::resume() {
    Q_D(Download);
    d->resume();
}

void
Download::start() {
    Q_D(Download);
    d->start();
}

void
Download::emitError(const QString& error) {
    Q_D(Download);
    d->emitError(error);
}

#include "moc_download.cpp"
