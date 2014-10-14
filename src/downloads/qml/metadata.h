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

#ifndef METADATA_H
#define METADATA_H
#include <ubuntu/transfers/metadata.h>

namespace Ubuntu {

namespace DownloadManager {

class Metadata : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(bool showInIndicator READ showInIndicator WRITE setShowInIndicator NOTIFY showIndicatorChanged)
    Q_PROPERTY(bool deflate READ deflate WRITE setDeflate NOTIFY deflateChanged)

 public:
    explicit Metadata(QObject* parent=0);
    Metadata(QVariantMap map, QObject* parent=0);

    QString title() const;
    void setTitle(QString title);

    bool showInIndicator() const;
    void setShowInIndicator(bool shown);

    bool deflate() const;
    void setDeflate(bool deflate);

    QVariantMap map() const;

 signals:
    void titleChanged();
    void showIndicatorChanged();
    void deflateChanged();

 private:
    Transfers::Metadata _metadata;
};

} // Ubuntu

} // DownloadManager


#endif
