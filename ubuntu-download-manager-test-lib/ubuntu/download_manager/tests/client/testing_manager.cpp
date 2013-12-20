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

#include "testing_manager.h"

TestingManager::TestingManager(QObject *parent) :
    QObject(parent) {
}

void
TestingManager::returnDBusErrors(bool errors) {
    _returnErrors = errors;
}

QDBusObjectPath
TestingManager::createDownload(DownloadStruct download) {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "createDownload");
    }
    return Manager::createDownload(download);
}

QDBusObjectPath
TestingManager::createDownloadGroup(StructList downloads,
                                    const QString& algorithm,
                                    bool allowed3G,
                                    const QVariantMap& metadata,
                                    StringMap headers) {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "createDownloadGroup");
    }
    return Manager::createDownloadGroup(downloads, algorithm, allowed3G,
        metadata, headers);
}

qulonglong
TestingManager::defaultThrottle() {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "defaultThrottle");
    }
    return Manager::defaultThrottle();
}

void
TestingManager::setDefaultThrottle(qulonglong speed) {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "setDefaultThrottle");
    }
    Manager::setDefaultThrottle(speed);
}

QList<QDBusObjectPath>
TestingManager::getAllDownloads() {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "getAllDownloads");
    }
    return Manager::getAllDownloads();
}

QList<QDBusObjectPath>
TestingManager::getAllDownloadsWithMetadata(const QString& name,
                                            const QString& value) {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "getAllDownloadsWithMetadata");
    }
    return Manager::getAllDownloadsWithMetadata(name, value);
}

void
TestingManager::exit() {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "exit");
        return;
    }
    Manager::exit();
}
