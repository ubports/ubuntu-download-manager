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

namespace Transfers {

const QString Metadata::COMMAND_KEY = "post-download-command";
const QString Metadata::COMMAND_FILE_KEY = "$file";
const QString Metadata::LOCAL_PATH_KEY = "local-path";
const QString Metadata::OBJECT_PATH_KEY = "objectpath";
const QString Metadata::TITLE_KEY = "title";
const QString Metadata::SHOW_IN_INDICATOR_KEY = "indicator-shown";
const QString Metadata::CLICK_PACKAGE_KEY = "click-package";

Metadata::Metadata() {
}

Metadata::Metadata(const QVariantMap map)
    : QVariantMap(map) {
}

QString
Metadata::command() const {
    return (contains(Metadata::COMMAND_KEY))?
        value(Metadata::COMMAND_KEY).toString():"";
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
        value(Metadata::LOCAL_PATH_KEY).toString():"";
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
        value(Metadata::OBJECT_PATH_KEY).toString():"";
}

void
Metadata::setObjectPath(const QString& path) {
    insert(Metadata::OBJECT_PATH_KEY, path);
}

bool
Metadata::hasObjectPath() const {
    return contains(Metadata::OBJECT_PATH_KEY);
}

QString
Metadata::title() const {
    return (contains(Metadata::TITLE_KEY))?
        value(Metadata::TITLE_KEY).toString():"";
}

void
Metadata::setTitle(const QString& title) {
    insert(Metadata::TITLE_KEY, title);
}

bool
Metadata::hasTitle() const {
    return contains(Metadata::TITLE_KEY);
}

bool
Metadata::showInIndicator() const {
    return (contains(Metadata::SHOW_IN_INDICATOR_KEY))?
        value(Metadata::SHOW_IN_INDICATOR_KEY).toBool():true;
}

void
Metadata::setShowInIndicator(bool shown) {
    insert(Metadata::SHOW_IN_INDICATOR_KEY, shown);
}

bool
Metadata::hasShowInIndicator() const {
    return contains(Metadata::SHOW_IN_INDICATOR_KEY);
}

QString
Metadata::clickPackage() const {
    return (contains(Metadata::CLICK_PACKAGE_KEY))?
        value(Metadata::CLICK_PACKAGE_KEY).toString():"";
}

void
Metadata::setClickPackage(const QString& click) {
    insert(Metadata::CLICK_PACKAGE_KEY, click);
}

bool
Metadata::hasClickPackage() const {
    return contains(Metadata::CLICK_PACKAGE_KEY);
}

}  // DownloadManager

}  // Ubuntu
