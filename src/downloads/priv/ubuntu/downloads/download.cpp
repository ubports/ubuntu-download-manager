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
#include "ubuntu/transfers/metadata.h"
#include "ubuntu/transfers/system/logger.h"
#include "download.h"

namespace Ubuntu {

namespace DownloadManager {

namespace Daemon {

Download::Download(const QString& id,
                   const QString& appId,
                   const QString& path,
                   bool isConfined,
                   const QString& rootPath,
                   const QVariantMap& metadata,
                   const QMap<QString, QString>& headers,
                   QObject* parent)
    : Transfer(id, appId, path, isConfined, rootPath, parent),
      _metadata(metadata),
      _headers(headers) {
}

Download::~Download() {
    // loop over the adaptors and call their deleteLater to ensure all
    // signals are dealt with
    foreach(const QString& interface, _adaptors.keys()) {
        _adaptors[interface]->deleteLater();
    }
}

void
Download::setAdaptor(const QString& interface, QObject* adaptor) {
    _adaptors[interface] = adaptor;
}

void
Download::emitError(const QString& errorStr) {
    setState(Download::ERROR);
    emit error(errorStr);
}

QString
Download::clickPackage() const {
    return (_metadata.contains(Metadata::CLICK_PACKAGE_KEY))?
        _metadata.value(Metadata::CLICK_PACKAGE_KEY).toString():"";
}

bool
Download::showInIndicator() const {
    return (_metadata.contains(Metadata::SHOW_IN_INDICATOR_KEY))?
        _metadata.value(Metadata::SHOW_IN_INDICATOR_KEY).toBool():false;
}

QString
Download::title() const {
    return (_metadata.contains(Metadata::TITLE_KEY))?
        _metadata.value(Metadata::TITLE_KEY).toString():"";
}

}  // Daemon

}  // DownloadManager

}  // Ubuntu
