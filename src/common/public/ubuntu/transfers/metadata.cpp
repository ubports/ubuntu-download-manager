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

#include <QProcessEnvironment>
#include <QCoreApplication>

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
const QString Metadata::DEFLATE_KEY = "deflate";
const QString Metadata::EXTRACT_KEY = "extract";
const QString Metadata::CUSTOM_PREFIX = "custom_";
const QString Metadata::APP_ID = "app-id";

namespace {
    const QString APP_ID_ENV = "APP_ID";
}

Metadata::Metadata() {
    auto environment = QProcessEnvironment::systemEnvironment();
    if (environment.contains(APP_ID_ENV)) {
        setOwner(environment.value(APP_ID_ENV));
    } else {
        setOwner(QCoreApplication::applicationFilePath());
    }
}

Metadata::Metadata(const QVariantMap map)
    : QVariantMap(map) {
    // check if the app id is present, if not, do it
    if (!hasOwner()) {
        auto environment = QProcessEnvironment::systemEnvironment();
        if (environment.contains(APP_ID_ENV)) {
            setOwner(environment.value(APP_ID_ENV));
        } else {
            setOwner(QCoreApplication::applicationFilePath());
        }
    }
}

QStringList
Metadata::command() const {
    return (contains(Metadata::COMMAND_KEY))?
        value(Metadata::COMMAND_KEY).toStringList():QStringList();
}

void
Metadata::setCommand(const QStringList& command) {
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
        value(Metadata::SHOW_IN_INDICATOR_KEY).toBool():false;
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

bool
Metadata::deflate() const {
    return (contains(Metadata::DEFLATE_KEY))?
        value(Metadata::DEFLATE_KEY).toBool():false;
}

void
Metadata::setDeflate(bool deflate) {
    insert(Metadata::DEFLATE_KEY, deflate);
}

bool
Metadata::hasDeflate() const {
    return contains(Metadata::DEFLATE_KEY);
}

bool
Metadata::extract() const {
    return (contains(Metadata::EXTRACT_KEY))?
        value(Metadata::EXTRACT_KEY).toBool():false;
}

void
Metadata::setExtract(bool extract) {
    insert(Metadata::EXTRACT_KEY, extract);
}

bool
Metadata::hasExtract() const {
    return contains(Metadata::EXTRACT_KEY);
}

QString
Metadata::destinationApp() const {
    return (contains(Metadata::APP_ID))?
           value(Metadata::APP_ID).toString():"";
}

void
Metadata::setOwner(const QString &id) {
    insert(Metadata::APP_ID, id);
}

bool
Metadata::hasOwner() const {
    return contains(Metadata::APP_ID);
}

QVariantMap
Metadata::custom() const {
    QVariantMap custom;
    foreach(QString key, keys()) {
        if (key.startsWith(CUSTOM_PREFIX)) {
            QString customKey = key;
            customKey.replace(CUSTOM_PREFIX, "");
            custom.insert(customKey, value(key));
        }
    }
    return custom;
}

void
Metadata::setCustom(QVariantMap custom) {
    // We can't send nested QVariantMaps over dbus, so flatten to one map
    foreach(QString key, custom.keys()) {
        insert(Metadata::CUSTOM_PREFIX + key, custom[key]);
    }
}

bool
Metadata::hasCustom() const {
    foreach(QString key, keys()) {
        if (key.startsWith(CUSTOM_PREFIX)) {
            return true;
        }
    }
    return false;
}

}  // DownloadManager

}  // Ubuntu
