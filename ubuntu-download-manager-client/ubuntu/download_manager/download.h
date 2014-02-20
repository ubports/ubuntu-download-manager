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

#ifndef UBUNTU_DOWNLOADMANAGER_CLIENT_DOWNLOAD_H
#define UBUNTU_DOWNLOADMANAGER_CLIENT_DOWNLOAD_H

#include <QObject>
#include <QVariantMap>
#include <QString>
#include <ubuntu/download_manager/common.h>

namespace Ubuntu {

namespace DownloadManager {

class Error;
class DOWNLOAD_MANAGER_EXPORT Download : public QObject {
    Q_OBJECT

 public:
    explicit Download(QObject* parent = 0)
        : QObject(parent) {}

    virtual void start() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual void cancel() = 0;

    virtual void allowMobileDownload(bool allowed) = 0;
    virtual bool isMobileDownloadAllowed() = 0;

    virtual void setThrottle(qulonglong speed) = 0;
    virtual qulonglong throttle() = 0;

    virtual QString id() const = 0;
    virtual QVariantMap metadata() = 0;
    virtual qulonglong progress() = 0;
    virtual qulonglong totalSize() = 0;

    virtual bool isError() const = 0;
    virtual Error* error() const = 0;

 signals:
    void canceled(bool success);
    void error(Error* error);
    void finished(const QString& path);
    void paused(bool success);
    void processing(const QString &path);
    void progress(qulonglong received, qulonglong total);
    void resumed(bool success);
    void started(bool success);

};

}  // Ubuntu

}  // DownloadManager

#endif  // UBUNTU_DOWNLOADMANAGER_CLIENT_DOWNLOAD_H
