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

#include "metadata.h"

namespace Ubuntu {

namespace DownloadManager {

Metadata::Metadata(QObject* parent)
    : QObject(parent) {
}

Metadata::Metadata(QVariantMap map, QObject* parent)
    : QObject(parent),
      _metadata(map) {
}

QString
Metadata::title() const {
    return _metadata.title();
}

void
Metadata::setTitle(QString title) {
    if (title != _metadata.title()) {
        _metadata.setTitle(title);
        emit titleChanged();
    }
}

bool
Metadata::showInIndicator() const {
    return _metadata.showInIndicator();
}

void
Metadata::setShowInIndicator(bool shown) {
    if (shown != _metadata.showInIndicator()) {
        _metadata.setShowInIndicator(shown);
        emit showIndicatorChanged();
    }
}

bool
Metadata::deflate() const {
    return _metadata.deflate();
}

void
Metadata::setDeflate(bool deflate) {
    if (deflate != _metadata.deflate()) {
        _metadata.setDeflate(deflate);
        emit deflateChanged();
    }
}

QVariantMap
Metadata::map() const {
    return QVariantMap(_metadata);
}

}

}
