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

#ifndef UDM_EXTRACTOR_DEFLATOR_H
#define UDM_EXTRACTOR_DEFLATOR_H

#include <QObject>
#include <QString>

namespace Ubuntu {

namespace DownloadManager {

namespace Extractor {

class Deflator : public QObject {
    Q_OBJECT

 public:
    Deflator(const QString& path, const QString& destination,
            QObject* parent=0);

    bool isError() const;
    QString lastError() const;

 public slots:  // NOLINT (whitespace/indent)
    virtual void deflate() = 0;

 protected:
    void setLastError(const QString& error);

 protected:
    QString _path = QString();
    QString _destination = QString();

 private:
    bool _isError = false;
    QString _error = QString();

};

}

}

}

#endif
