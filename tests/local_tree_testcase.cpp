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

#include "local_tree_testcase.h"
#define TEST_DAEMON "../src/downloads/test-daemon/ubuntu-download-manager-test-daemon"
#define LARGE_FILE "otasigned.zip"
#define SMALL_FILE "index.json"

LocalTreeTestCase::LocalTreeTestCase(const QString& testName,
                                     QObject* parent)
    : DaemonTestCase(testName, TEST_DAEMON, parent) {
}

QUrl
LocalTreeTestCase::largeFileUrl() {
    auto url = serverUrl().toString() + "/" + LARGE_FILE;
    return QUrl(url);
}

QUrl
LocalTreeTestCase::smallFileUrl() {
    auto url = serverUrl().toString() + "/" + SMALL_FILE;
    return QUrl(url);
}

void
LocalTreeTestCase::init() {
    DaemonTestCase::init();

    // add to testing files, one large and one smaller
    QString largeFile = dataDirectory() + "/" + QString(LARGE_FILE);
    addFileToHttpServer(largeFile);

    QString smallFile = dataDirectory() + "/" + QString(SMALL_FILE);
    addFileToHttpServer(smallFile);
    Ubuntu::DownloadManager::Logging::Logger::init(
        Ubuntu::DownloadManager::Logging::Logger::Debug, "");
}
