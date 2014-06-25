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

#ifndef DOWNLOAD_QML_LOGGER_H
#define DOWNLOAD_QML_LOGGER_H

namespace Ubuntu {

namespace DownloadManager {

class Logger : public QObject {
    Q_OBJECT
    Q_ENUMS(Level)
    Q_PROPERTY(QString path READ path WRITE setPath)
    Q_PROPERTY(Level level READ level WRITE setLevel)

 public:

    enum Level {
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    explicit Logger(QObject* parent=0);
    Logger(const QString& path, QObject* parent=0);

    Q_INVOKABLE void init(Level lvl);
    Q_INVOKABLE void stop(Level lvl);

 private:
    QString m_path;  // path used to log
};

}  // DownloadManager

}  // Ubuntu 

#endif // DOWNLOAD_QML_LOGGER_H 
