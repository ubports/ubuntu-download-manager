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

#ifndef DOWNLOAD_ERROR_H
#define DOWNLOAD_ERROR_H

#include <QObject>
#include <QString>

namespace Ubuntu {

namespace DownloadManager {

class DownloadError : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString type READ type NOTIFY typeChanged)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)

public:
    explicit DownloadError(QObject *parent = 0);

    QString type() const { return m_type; }
    QString message() const { return m_message; }

    void setType(QString type) { m_type = type; emit typeChanged(); }
    void setMessage(QString message) { m_message = message; emit messageChanged(); }

signals:
    void typeChanged();
    void messageChanged();

private:
    QString m_message;
    QString m_type;

};

}
}

#endif // DOWNLOAD_ERROR_H
