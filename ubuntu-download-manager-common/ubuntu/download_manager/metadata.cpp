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

const QString Metadata::COMMAND_KEY = "post-download-command";
const QString Metadata::COMMAND_FILE_KEY = "$file";
const QString Metadata::LOCAL_PATH_KEY = "local-path";
const QString Metadata::OBJECT_PATH_KEY = "objectpath";

QString
Metadata::command() const {
    return (contains(Metadata::COMMAND_KEY))?
        value(COMMAND_KEY).toString():"";
}

void
Metadata::setCommand(const QString& command) {
    insert(Metadata::COMMAND_KEY, command);
}

bool
Metadata::hasCommand() const {
    return contains(Metadata::COMMAND_KEY);
}

QString
Metadata::localPath() const {
    return (contains(Metadata::LOCAL_PATH_KEY))?
        value(LOCAL_PATH_KEY).toString():"";
}

void
Metadata::setLocalPath(const QString& localPath) {
    insert(Metadata::LOCAL_PATH_KEY, localPath);
}

bool
Metadata::hasLocalPath() const {
    return contains(Metadata::LOCAL_PATH_KEY);
}

QString
Metadata::objectPath() const {
    return (contains(Metadata::OBJECT_PATH_KEY))?
        value(OBJECT_PATH_KEY).toString():"";
}

void
Metadata::setObjectPath(const QString& path) {
    insert(Metadata::OBJECT_PATH_KEY, path);
}

bool
Metadata::hasObjectPath() const {
    return contains(Metadata::OBJECT_PATH_KEY);
}

}  // DownloadManager

}  // Ubuntu
