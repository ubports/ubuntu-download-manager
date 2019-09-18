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

#ifndef UDM_EXTRACTOR_FACTORY_H
#define UDM_EXTRACTOR_FACTORY_H

namespace Ubuntu {

namespace DownloadManager {

namespace Extractor {

class Deflator;

class Factory : public QObject {
    Q_OBJECT

 public:
    explicit Factory(QObject* parent=0);

    Deflator* deflator(const QString& id, const QString& path,
            const QString& destination);

    bool isError() const;
    QString lastError() const;

 private:
    QString _error = QString();
};

}

}

}

#endif
