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

#include <QDebug>
#include <ubuntu/downloads/queue.h>
#include <ubuntu/downloads/download.h>
#include <ubuntu/downloads/download_adaptor.h>
#include <ubuntu/downloads/file_download.h>
#include "testing_file_download.h"
#include "testing_manager.h"

void
TestingManager::returnDBusErrors(bool errors) {
    qDebug() << "Manager will return DBus errors" << errors;
    _returnErrors = errors;
    // set all the downloads to return errors
    auto q = queue();
    foreach(Download* down, q->downloads().values()) {
        auto testDown = qobject_cast<TestingFileDownload*>(down);
        if (testDown != nullptr) {
            testDown->returnDBusErrors(errors);
        }
    }
}

void
TestingManager::returnHttpError(const QString &download,
                                HttpErrorStruct error) {
    auto q = queue();
    foreach(Download* down, q->downloads().values()) {
        auto testDown = qobject_cast<TestingFileDownload*>(down);
        if (testDown != nullptr && testDown->url() == download) {
            testDown->returnHttpError(error);
        }
    }
}

void
TestingManager::returnNetworkError(const QString &download,
                                   NetworkErrorStruct error) {
    auto q = queue();
    foreach(Download* down, q->downloads().values()) {
        auto testDown = qobject_cast<TestingFileDownload*>(down);
        if (testDown != nullptr && testDown->url() == download) {
            testDown->returnNetworkError(error);
        }
    }
}

void
TestingManager::returnProcessError(const QString &download,
                                   ProcessErrorStruct error) {
    auto q = queue();
    foreach(Download* down, q->downloads().values()) {
        auto testDown = qobject_cast<TestingFileDownload*>(down);
        if (testDown != nullptr && testDown->url() == download) {
            testDown->returnProcessError(error);
        }
    }
}

void
TestingManager::returnAuthError(const QString &download,
                                AuthErrorStruct error) {
    auto q = queue();
    foreach(Download* down, q->downloads().values()) {
        auto testDown = qobject_cast<TestingFileDownload*>(down);
        if (testDown != nullptr && testDown->url() == download) {
            testDown->returnAuthError(error);
        }
    }
}

QDBusObjectPath
TestingManager::registerDownload(Download* download) {
    QDBusObjectPath path;
    auto fileDown = qobject_cast<FileDownload*>(download);
    if (fileDown != nullptr) {
    qDebug() << "Register testing file";
    auto testDown = new TestingFileDownload(fileDown);
        auto downAdaptor = new DownloadAdaptor(testDown);
    Q_UNUSED(downAdaptor);
        // create wrapper and call parent class
        path = DownloadManager::registerDownload(testDown);
    } else {
        path = DownloadManager::registerDownload(download);
    }
    // create an adaptor so that we can be exposed to Dbus
    qDebug() << "Path" << path.path();
    return path;
}

QDBusObjectPath
TestingManager::createDownload(DownloadStruct download) {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "createDownload");
    }
    return DownloadManager::createDownload(download);
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
    return DownloadManager::createDownloadGroup(downloads, algorithm, allowed3G,
        metadata, headers);
}

qulonglong
TestingManager::defaultThrottle() {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "defaultThrottle");
    }
    return DownloadManager::defaultThrottle();
}

void
TestingManager::setDefaultThrottle(qulonglong speed) {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "setDefaultThrottle");
    }
    DownloadManager::setDefaultThrottle(speed);
}

void
TestingManager::allowGSMDownload(bool allowed) {
    if (calledFromDBus() && _returnErrors) {
        qDebug() << "Send error";
        sendErrorReply(QDBusError::InvalidMember,
        "setDefaultThrottle");
    }
    DownloadManager::allowGSMDownload(allowed);
}

bool
TestingManager::isGSMDownloadAllowed() {
    if (calledFromDBus() && _returnErrors) {
        qDebug() << "Send error";
        sendErrorReply(QDBusError::InvalidMember,
        "setDefaultThrottle");
    }
    return DownloadManager::isGSMDownloadAllowed();
}

QList<QDBusObjectPath>
TestingManager::getAllDownloads() {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "getAllDownloads");
    }
    return DownloadManager::getAllDownloads();
}

QList<QDBusObjectPath>
TestingManager::getAllDownloadsWithMetadata(const QString& name,
                                            const QString& value) {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "getAllDownloadsWithMetadata");
    }
    return DownloadManager::getAllDownloadsWithMetadata(name, value);
}

void
TestingManager::exit() {
    if (calledFromDBus() && _returnErrors) {
        sendErrorReply(QDBusError::InvalidMember,
        "exit");
        return;
    }
    DownloadManager::exit();
}
