/*
 * Copyright 2013 Canonical Ltd.
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

#ifndef DOWNLOADER_LIB_METADATA_H
#define DOWNLOADER_LIB_METADATA_H

#include <QMap>
#include <QVariant>

namespace Ubuntu {

namespace DownloadManager {

class Metadata : public QVariantMap {

 public:

    static const QString COMMAND_KEY;
    static const QString COMMAND_FILE_KEY;
    static const QString LOCAL_PATH_KEY;
    static const QString OBJECT_PATH_KEY;

    // accessors to simplify the use of the metadata
    QString command() const;
    void setCommand(const QString& command);
    bool hasCommand() const;

    QString localPath() const;
    void setLocalPath(const QString& localPath);
    bool hasLocalPath() const;

    QString objectPath() const;
    void setObjectPath(const QString& path);
    bool hasObjectPath() const;
};

}  // DownloadManager

}  // Ubuntu

#endif // METADATA_H
