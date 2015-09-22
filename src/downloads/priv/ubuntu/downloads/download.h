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

#pragma once

#include <QNetworkAccessManager>
#include <QObject>
#include <QProcess>
#include <ubuntu/transfers/transfer.h>
#include <ubuntu/transfers/metadata.h>
#include <ubuntu/download_manager/metatypes.h>
#include "ubuntu/transfers/system/process_factory.h"
#include "ubuntu/transfers/system/request_factory.h"


namespace Ubuntu {

using namespace Transfers;
using namespace Transfers::System;

namespace DownloadManager {

namespace Daemon {

class Download : public Transfer {
    Q_OBJECT
    Q_PROPERTY(QString ClickPackage READ clickPackage)
    Q_PROPERTY(bool ShowInIndicator READ showInIndicator)
    Q_PROPERTY(QString Title READ title)
    Q_PROPERTY(QString DownloadOwner READ destinationApp)

 public:
    Download(const QString& id,
             const QString& appId,
             const QString& path,
             bool isConfined,
             const QString& rootPath,
             const QVariantMap& metadata,
             const QMap<QString, QString>& headers,
             QObject* parent = 0);

    virtual ~Download();

    QObject* adaptor(const QString& interface) const {
        return _adaptors[interface];
    }

    void setAdaptor(const QString& interface, QObject* adaptor);

    virtual bool pausable() {
        return true;
    }

 public slots:  // NOLINT(whitespace/indent)
    // slots that are exposed via dbus, they just change the state,
    // the downloader takes care of the actual download operations
    virtual QVariantMap metadata() const {
        return _metadata;
    }

    virtual void allowGSMDownload(bool allowed) {
        // rename the transfer method
        Transfer::allowGSMData(allowed);
    }

    virtual bool isGSMDownloadAllowed() {
        return Transfer::isGSMDataAllowed();
    }

    virtual StringMap headers() const {
        return _headers;
    }

    virtual void setHeaders(StringMap headers) {
        _headers = headers;
    }

    virtual void setMetadata(const QVariantMap& data) {
        _metadata = data;
    }

    virtual QString destinationApp() {
        return _destinationApp;
    }

    virtual void setDownloadOwner(const QString& owner) {
        _destinationApp = owner;
    }

    // slots to be implemented by the children
    virtual qulonglong progress() = 0;
    virtual qulonglong totalSize() = 0;

 signals:
    // signals that are exposed via dbus
    void processing(const QString& file);
    void progress(qulonglong received, qulonglong total);

 protected:
    virtual void emitError(const QString& error);
    virtual QString clickPackage() const;
    virtual bool showInIndicator() const;
    virtual QString title() const;

 protected:
    QVariantMap _metadata;

 private:
    QString _destinationApp = QString::null;
    QMap<QString, QString> _headers;
    QMap<QString, QObject*> _adaptors;
};

}  // Daemon

}  // DownloadManager

}  // Ubuntu

