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

#include <QStringList>
#include "ubuntu/transfers/system/logger.h"
#include "download.h"

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

Download::Download(const QString& id,
                   const QString& path,
                   bool isConfined,
                   const QString& rootPath,
                   const QVariantMap& metadata,
                   const QMap<QString, QString>& headers,
                   QObject* parent)
    : Transfer(id, path, isConfined, parent),
      _rootPath(rootPath),
      _metadata(metadata),
      _headers(headers) {
}

Download::~Download() {
    if (_adaptor != nullptr) {
        _adaptor->deleteLater();
    }
}

void
Download::setAdaptor(QObject* adaptor) {
    _adaptor = adaptor;
}

void
Download::emitError(const QString& errorStr) {
    qDebug() << __PRETTY_FUNCTION__ << errorStr;
    setState(Download::ERROR);
    emit error(errorStr);
}

}  // Daemon

}  // DownloadManager

}  // Ubuntu
